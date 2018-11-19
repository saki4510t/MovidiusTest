package com.serenegiant.ncsdk;

import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.RawRes;
import android.util.Log;

import com.serenegiant.usb.USBMonitor;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class Movidius {
	private static final boolean DEBUG = true; // set false on production
	private static final String TAG = Movidius.class.getSimpleName();

	private static boolean isLoaded;
	static {
		if (!isLoaded) {
			System.loadLibrary("c++_shared");
			System.loadLibrary("common");
			System.loadLibrary("movidius");
			isLoaded = true;
		}
	}

//	UsbDevice[mName=/dev/bus/usb/001/002,mVendorId=999,mProductId=8528,mClass=0,mSubclass=0,mProtocol=0,mManufacturerName=Movidius Ltd.,mProductName=Movidius MA2X5X,mVersion=2.0,mSerialNumber=03e72150,mConfigurations=[
//	    UsbConfiguration[mId=1,mName=null,mAttributes=128,mMaxPower=250,mInterfaces=[
//	    UsbInterface[mId=0,mAlternateSetting=0,mName=null,mClass=255,mSubclass=17,mProtocol=255,mEndpoints=[
//	    UsbEndpoint[mAddress=129,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=1,mAttributes=2,mMaxPacketSize=512,mInterval=0]]]]

	/** 未初期化 */
	private static final int STATE_UNINITIALIZED	= 0x00000000;
	/** 初期化中フラグ */
	private static final int STATE_INITIALIZING		= 0x00000001;
	/** 初期化済フラグ */
	private static final int STATE_INITIALIZED		= 0x00000010;
	/** NCSと接続中フラグ */
	private static final int STATE_CONNECTING		= 0x00000100;
	/** NCSと接続完了・使用可能フラグ */
	private static final int STATE_CONNECTED		= 0x00000200;

	private final ReentrantReadWriteLock mSensorLock = new ReentrantReadWriteLock();
	private final Lock mReadLock = mSensorLock.readLock();
	private final Lock mWriteLock = mSensorLock.writeLock();
	private int mState = STATE_UNINITIALIZED;

	private final WeakReference<Context> mWeakContext;
	protected long mNativePtr;
	protected USBMonitor.UsbControlBlock mCtrlBlock;
	
	/**
	 * コンストラクタ
	 * @throws UnsupportedOperationException
	 */
	public Movidius(@NonNull final Context context)
		throws UnsupportedOperationException {

		setState(STATE_INITIALIZING);
		mWeakContext = new WeakReference<Context>(context);
		mNativePtr = nativeCreate();
		if (mNativePtr == 0) {
			throw new UnsupportedOperationException();
		}
		setState(STATE_INITIALIZED);
	}
	
	@Override
	protected void finalize() throws Throwable {
		try {
			release();
		} finally {
			super.finalize();
		}
	}

//================================================================================
	@NonNull
	protected Context requireContext() throws IllegalStateException {
		final Context context = mWeakContext.get();
		if (context == null) {
			throw new IllegalStateException("already released?");
		}
		return context;
	}

	@Nullable
	protected Context getContext() {
		return mWeakContext.get();
	}

	/**
	 * 現在のステートを取得
	 * @return
	 */
	protected int state() {
		mReadLock.lock();
		try {
			return mState;
		} finally {
			mReadLock.unlock();
		}
	}

	/**
	 * ステートをセット
	 * @param newState
	 * @return ステートが変化した時はtrue
	 */
	protected boolean setState(final int newState) {
		boolean changed;
		mWriteLock.lock();
		try {
			changed = mState != newState;
			mState = newState;
		} finally {
			mWriteLock.unlock();
		}
		return changed;
	}

	/**
	 * 現在のステートにマスクを適用した後新しいステートを付加する
	 * state = (state & mask) | newState
	 * @param newState
	 * @param mask
	 * @return ステートが変化した時はtrue
	 */
	protected boolean setState(final int newState, final int mask) {
		boolean changed;
		mWriteLock.lock();
		try {
			final int state = mState & mask;
			changed = state != newState;
			if (changed) {
				mState = state | newState;
			}
		} finally {
			mWriteLock.unlock();
		}
		return changed;
	}

//================================================================================
	public void release() {
		close();
		if (mNativePtr != 0) {
			nativeDestroy(mNativePtr);
			mNativePtr = 0;
		}
	}

	@Override
	public synchronized boolean equals(final Object obj) {
		if (obj instanceof UsbDevice) {
			return obj.equals(mCtrlBlock != null ? mCtrlBlock.getDevice() : null);
		} else if (obj instanceof USBMonitor.UsbControlBlock) {
			return obj.equals(mCtrlBlock);
		}
		return super.equals(obj);
	}

	public synchronized void open(final USBMonitor.UsbControlBlock ctrlBlock) {
		int result;
		try {
			mCtrlBlock = ctrlBlock.clone();
			result = nativeConnect(mNativePtr);
		} catch (final Exception e) {
			result = -1;
			Log.w(TAG, e);
		}
		if (result != 0) {
			throw new UnsupportedOperationException("open failed:result=" + result);
		}
	}

	public synchronized void close() {
		if (mNativePtr != 0) {
			nativeDisConnect(mNativePtr);
		}
	}
	
	private void usbBoot(@RawRes final int mvcmd)
		throws IllegalStateException, IllegalArgumentException {

		@NonNull
		final Context context = requireContext();
		final InputStream in = new BufferedInputStream(
			context.getResources().openRawResource(mvcmd));
		// mvcmdファイルのサイズが不明なのでとりあえずByteArrayOutputStreamへ
		// 書き込んでbyte配列に変える
		final byte[] buf = new byte[4096];
		final ByteArrayOutputStream out = new ByteArrayOutputStream();
		try {
			try {
				for (; ; ) {
					final int readBytes = in.read(buf);
					if (readBytes > 0) {
						out.write(buf, 0, readBytes);
					} else {
						break;
					}
				}
			} catch (final IOException e) {
				Log.w(TAG, e);
			}
		} finally {
			try {
				in.close();
			} catch (final IOException e) {
				Log.w(TAG, e);
				throw new IllegalArgumentException(e);
			}
		}
		if (out.size() > 0) {
			final byte[] fw = out.toByteArray();
			if (DEBUG) Log.v(TAG, "usbBoot:" + Arrays.toString(fw));
			// FIXME 未実装 ncsへの書き込み処理
		} else {
			throw new IllegalArgumentException("failed to read mvcmd file or empty");
		}
	}

//================================================================================
// ここから下はnative側で実装するはずのメソッド
	private native long nativeCreate();
	private native void nativeDestroy(long id_ncs);
	private native int nativeConnect(final long id_ncs);
	private native int nativeDisConnect(long id_ncs);
}
