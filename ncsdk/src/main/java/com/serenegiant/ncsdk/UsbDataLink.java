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
import java.util.Arrays;
import java.util.Locale;

public class UsbDataLink extends NativeObject implements IDataLink {
	private static final boolean DEBUG = true; // set false on production
	private static final String TAG = UsbDataLink.class.getSimpleName();

	static {
		NativeLoader.loadNative();
	}

	private static final int VID_MOVIDIUS = 0x03e7;
	// Once opened in VSC mode, VID/PID change
	private static final int VID_MOVIDIUS_USB_BOOT = VID_MOVIDIUS;
	/**
	 * bootしたあとのpid
	 * これはMovidius Neural Compute Stick とNeural Compute Stick 2で共通
	 */
	private static final int PID_MOVIDIUS_USB_BOOT = 0xf63b; // ==63035; これは

	private static final int PID_SUPPORTED[] = {
		0x2150, // == 8528; ma2450, Myriad2v2 ROM, Movidius Neural Compute Stick
//		0x2485,	// == 9349; ma2480, Neural Compute Stick 2
	};
// [Movidius Neural Compute Stick]
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

// [Neural Compute Stick 2]
// 最初に接続した時はこのUsbDeviceが来る
//	UsbDevice[mName=/dev/bus/usb/001/002,mVendorId=999,mProductId=9349,mClass=0,mSubclass=0,mProtocol=0,mManufacturerName=Movidius Ltd.,mProductName=Movidius MyriadX,mVersion=2.0,mSerialNumber=03e72485,mConfigurations=[
//	    UsbConfiguration[mId=1,mName=null,mAttributes=128,mMaxPower=250,mInterfaces=[
//	    UsbInterface[mId=0,mAlternateSetting=0,mName=null,mClass=255,mSubclass=17,mProtocol=255,mEndpoints=[
//	    UsbEndpoint[mAddress=129,mAttributes=2,mMaxPacketSize=512,mInterval=0]
//	    UsbEndpoint[mAddress=1,mAttributes=2,mMaxPacketSize=512,mInterval=0]]]]

	/** NCSと接続中フラグ */
	private static final int STATE_CONNECTING		= 0x00000100;
	/** NCSと接続完了・使用可能フラグ */
	private static final int STATE_CONNECTED		= 0x00000200;

	protected USBMonitor.UsbControlBlock mCtrlBlock;
	
	/**
	 * コンストラクタ
	 * @throws UnsupportedOperationException
	 */
	public UsbDataLink(@NonNull final Context context)
		throws UnsupportedOperationException {

		super(context);
	}
	

//================================================================================
	@Override
	public void release() {
		close();
		super.release();
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
		return (mCtrlBlock != null)
			&& (mCtrlBlock.getProductId() == PID_MOVIDIUS_USB_BOOT);
	}

	public synchronized void open(
		@NonNull final USBMonitor.UsbControlBlock ctrlBlock) {

		int result = -1;
		try {
			mCtrlBlock = ctrlBlock.clone();
			final int vid = mCtrlBlock.getVenderId();
			final int pid = mCtrlBlock.getProductId();
			
			if ((vid == VID_MOVIDIUS_USB_BOOT)
				&& (pid == PID_MOVIDIUS_USB_BOOT)) {
			
				result = nativeConnect(mNativePtr, mCtrlBlock.getFileDescriptor());
			} else if (vid == VID_MOVIDIUS) {
				boolean found = false;
				for (int i = PID_SUPPORTED.length - 1; i >= 0; i--) {
					if (pid == PID_SUPPORTED[i]) {
						found = true;
						break;
					}
				}
				if (found) {
					// FIXME Neural Compute Stick2だとncsdk2.xにしないとだめかも
					usbBoot(mCtrlBlock, R.raw.mvncapi);
					result = 0;
				}
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
	
	public synchronized void reset()
		throws IllegalStateException, IllegalArgumentException {

		if (mNativePtr != 0) {
			final int result = nativeReset(mNativePtr);
			if (result != 0) {
				throw new IllegalArgumentException("nativeReset returned with err " + result);
			}
		} else {
			throw new IllegalStateException("already released?");
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
		// 書き込んでbyte配列に変える FIXME maxPacketSize毎に読み込んで送ればいい気がする
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
	private UsbEndpoint findOutEndpoint(
		@NonNull final USBMonitor.UsbControlBlock ctrlBlock) {

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
	@Override
	protected native long nativeCreate();
	@Override
	protected native void nativeDestroy(final long id_ncs);

	private native int nativeConnect(final long id_ncs, final int fd);
	private native int nativeDisConnect(final long id_ncs);
	private native int nativeReset(final long id_ncs);
}
