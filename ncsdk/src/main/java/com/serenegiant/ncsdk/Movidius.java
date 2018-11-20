package com.serenegiant.ncsdk;

import android.content.Context;
import android.hardware.usb.UsbConfiguration;
import android.hardware.usb.UsbConstants;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbInterface;
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
import java.util.Locale;
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

	private static final int VID_MOVIDIUS = 0x03e7;
	private static final int PID_MOVIDIUS = 0x2150; // ==8528; Myriad2v2 ROM
	// Once opened in VSC mode, VID/PID change
	private static final int VID_MOVIDIUS_USB_BOOT = VID_MOVIDIUS;
	private static final int PID_MOVIDIUS_USB_BOOT = 0xf63b; // ==63035;

// 最初に接続した時はこのUsbDeviceが来る
//	UsbDevice[mName=/dev/bus/usb/001/002,mVendorId=999,mProductId=8528,mClass=0,mSubclass=0,mProtocol=0,mManufacturerName=Movidius Ltd.,mProductName=Movidius MA2X5X,mVersion=2.0,mSerialNumber=03e72150,mConfigurations=[
//	    UsbConfiguration[mId=1,mName=null,mAttributes=128,mMaxPower=250,mInterfaces=[
//	    UsbInterface[mId=0,mAlternateSetting=0,mName=null,mClass=255,mSubclass=17,mProtocol=255,mEndpoints=[
//	    UsbEndpoint[mAddress=129,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=1,mAttributes=2,mMaxPacketSize=512,mInterval=0]]]]
// MvNCAPI.mvcmdを書き込むと一旦切断されてからこのUsbDeviceが来る
//	UsbDevice[mName=/dev/bus/usb/001/003,mVendorId=999,mProductId=63035,mClass=0,mSubclass=0,mProtocol=0,mManufacturerName=Movidius,mProductName=VSC Loopback Device,mVersion=2.16,mSerialNumber=DDB85DD564E710,mConfigurations=[
//	    UsbConfiguration[mId=1,mName=null,mAttributes=192,mMaxPower=3,mInterfaces=[
//	    UsbInterface[mId=0,mAlternateSetting=0,mName=Bulk Data,mClass=255,mSubclass=0,mProtocol=0,mEndpoints=[
//	    UsbEndpoint[mAddress=1,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=129,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=2,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=130,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=3,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=131,mAttributes=2,mMaxPacketSize=512,mInterval=0]]]]

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

	public synchronized boolean isBooted() {
		return (mCtrlBlock != null) && (mCtrlBlock.getProductId() == PID_MOVIDIUS_USB_BOOT);
	}

	public synchronized void open(
		@NonNull final USBMonitor.UsbControlBlock ctrlBlock) {

		int result = -1;
		
		try {
			mCtrlBlock = ctrlBlock.clone();
			final int vid = mCtrlBlock.getVenderId();
			final int pid = mCtrlBlock.getProductId();
			if ((vid == VID_MOVIDIUS) && (pid == PID_MOVIDIUS)) {
				usbBoot(mCtrlBlock, R.raw.mvncapi);
				result = 0;
			} else if ((vid == VID_MOVIDIUS_USB_BOOT)
				&& (pid == PID_MOVIDIUS_USB_BOOT)) {

				result = nativeConnect(mNativePtr, mCtrlBlock.getFileDescriptor());
			}
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
	
	private void usbBoot(@NonNull final USBMonitor.UsbControlBlock ctrlBlock,
		@RawRes final int mvcmd)
			throws IllegalStateException, IllegalArgumentException {

		if (DEBUG) Log.v(TAG, "usbBoot:");
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
			final int sendSize = fw.length;
			if (DEBUG) Log.v(TAG, "usbBoot:" + Arrays.toString(fw));
			final UsbEndpoint endpoint = findOutEndpoint(ctrlBlock);
			if (endpoint != null) {
				// ncsへの書き込み処理
				final int maxPacketSize = endpoint.getMaxPacketSize();
				int offset = 0;
				for (; offset < sendSize; ) {
					int wb = sendSize - offset;
					if (wb > maxPacketSize) {
						wb = maxPacketSize;
					}
					final int writeBytes = ctrlBlock.bulkTransfer(endpoint, fw, offset, wb, 2000);
					if (writeBytes <= 0) {
						break;
					}
					offset += writeBytes;
				}
				if (DEBUG) Log.v(TAG,
					String.format(Locale.US, "usbBoot:write %d/%d", offset, sendSize));
				// ncsへの書き込みが終わると一旦切断されてから別のpidとして再接続される
			} else {
				throw new IllegalArgumentException("specific device is not Movidius?");
			}
		} else {
			throw new IllegalArgumentException("failed to read mvcmd file or empty");
		}
	}

	@Nullable
	private UsbEndpoint findOutEndpoint(@NonNull final USBMonitor.UsbControlBlock ctrlBlock) {
		if (DEBUG) Log.v(TAG, "findOutEndpoint:");
		UsbEndpoint result = null;
		final UsbDevice device = ctrlBlock.getDevice();
		if (device != null) {
			final int numConfigs = device.getConfigurationCount();
			for (int i = 0; i < numConfigs; i++) {
				final UsbConfiguration config = device.getConfiguration(i);
				final int numIntfs = config.getInterfaceCount();
				for (int j = 0; j < numIntfs; j++) {
					final UsbInterface intf = config.getInterface(j);
					final int numEndpoints = intf.getEndpointCount();
					for (int k = 0; k < numEndpoints; k++) {
						final UsbEndpoint endpoint = intf.getEndpoint(k);
						final int dir = endpoint.getDirection();
						if (dir == UsbConstants.USB_DIR_OUT) {
							result = endpoint;
							break;
						}
					}
				}
			}
		}
		
		if (DEBUG) Log.v(TAG, "findOutEndpoint:" + result);
		return result;
	}

//================================================================================
// ここから下はnative側で実装するはずのメソッド
	private native long nativeCreate();
	private native void nativeDestroy(long id_ncs);
	private native int nativeConnect(final long id_ncs, final int fd);
	private native int nativeDisConnect(long id_ncs);
}
