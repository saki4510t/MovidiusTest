package com.serenegiant.movidiustest;

import android.Manifest;
import android.annotation.SuppressLint;
import android.hardware.usb.UsbDevice;
import android.os.Handler;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.serenegiant.dialog.MessageDialogFragmentV4;
import com.serenegiant.usb.DeviceFilter;
import com.serenegiant.usb.USBMonitor;
import com.serenegiant.utils.BuildCheck;
import com.serenegiant.utils.PermissionCheck;
import com.serenegiant.uvc.CameraDialogV4;
import com.serenegiant.uvc.ICameraDialogListener;

import java.util.List;

public class MainActivity extends AppCompatActivity
	implements ICameraDialogListener, MessageDialogFragmentV4.MessageDialogListener {

	private static final boolean DEBUG = true;
	private static final String TAG = MainActivity.class.getSimpleName();
	
	// カメラステート
	private static final int CAMERA_NON = 0;
	private static final int CAMERA_TRYOPEN = 1;
	private static final int CAMERA_INITIALIZING = 2;
	private static final int CAMERA_ACTIVE = 3;
	private static final int CAMERA_PREVIEWING = 4;

	/**
	 * 非同期で実行するためのHandler(UIスレッドじゃないよ)
	 */
    private Handler mAsyncHandler;
	private long mWorkerThreadID;
	/**
	 * UI操作のためのHandler
	 */
	private final Handler mUIHandler = new Handler();
	private final long mUIThreadID = Thread.currentThread().getId();
	private final Object mSync = new Object();

	private USBMonitor mUSBMonitor;
	private View mRootView;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		initView();
		if (mUSBMonitor == null) {
			mUSBMonitor = new USBMonitor(this, mOnDeviceConnectListener);
			final List<DeviceFilter> filters
				= DeviceFilter.getDeviceFilters(this, R.xml.device_filter_exclude);
			mUSBMonitor.setDeviceFilter(filters);
		}
	}
	
	@Override
	protected final void onStart() {
		super.onStart();
		if (DEBUG) Log.v(TAG, "onStart:");
		if (BuildCheck.isAndroid7()) {
			internalOnResume();
		}
	}

	@Override
	protected final void onResume() {
		super.onResume();
		if (DEBUG) Log.v(TAG, "onResume:");
		if (!BuildCheck.isAndroid7()) {
			internalOnResume();
		}
	}

	@Override
	protected final void onPause() {
		if (DEBUG) Log.v(TAG, "onPause:");
		if (!BuildCheck.isAndroid7()) {
			internalOnPause();
		}
		super.onPause();
	}

	@Override
	protected final void onStop() {
		if (DEBUG) Log.v(TAG, "onStop:");
		if (BuildCheck.isAndroid7()) {
			internalOnPause();
		}
		super.onStop();
	}

	@Override
	protected void onDestroy() {
		if (DEBUG) Log.v(TAG, "onDestroy:");
		close(false);
		if (mUSBMonitor != null) {
			mUSBMonitor.destroy();
			mUSBMonitor = null;
		}
		if (mAsyncHandler != null) {
			try {
				mAsyncHandler.getLooper().quit();
			} catch (final Exception e) {
				// ignore
			}
			mAsyncHandler = null;
		}
		super.onDestroy();
	}

	protected void internalOnResume() {
		if (DEBUG) Log.v(TAG, "internalOnResume:");
		if ((mUSBMonitor != null) && !mUSBMonitor.isRegistered()) {
			mUSBMonitor.register();
		}
	}

	protected void internalOnPause() {
		if (DEBUG) Log.v(TAG, "internalOnPause:");
		try {
			if ((mUSBMonitor != null) && mUSBMonitor.isRegistered()) {
				mUSBMonitor.unregister();
			}
		} catch (final Exception e) {
			Log.w(TAG, e);
		}
		cancelMessage();
	}

	@Override
	public USBMonitor getUSBMonitor() {
		return mUSBMonitor;
	}
	
//================================================================================
	/**
	 * UIスレッド上で指定したRunnableを実行する
	 * 未実行の同じRunnableがあればキャンセルされる(後から指定した方のみ実行される)
	 * @param task
	 * @param delayMillis ミリ秒
	 */
	protected final synchronized void runOnUiThread(
		final Runnable task, final long delayMillis) {

//		if (DEBUG) Log.v(TAG, "runOnUiThread:task=" + task + ", delayMillis=" + delayMillis);
		if (task == null) return;
		try {
			mUIHandler.removeCallbacks(task);
			if (delayMillis > 0) {
				mUIHandler.postDelayed(task, delayMillis);
			} else if (mUIThreadID == Thread.currentThread().getId()) {
				task.run();
			} else {
				mUIHandler.post(task);
			}
		} catch (final Exception e) {
			// ignore
		}
	}

	/**
	 * 指定したRunnableをUIスレッド上で実行予定であればキャンセルする
	 * @param task
	 */
	protected final synchronized void removeFromUiThread(final Runnable task) {
		if (task == null) return;
		try {
			mUIHandler.removeCallbacks(task);
		} catch (final Exception e) {
			// ignore
		}
	}

	/**
	 * ワーカースレッド上で指定したRunnableを実行する
	 * 未実行の同じRunnableがあればキャンセルされる(後から指定した方のみ実行される)
	 * @param task
	 * @param delayMillis
	 */
	protected final synchronized void queueEvent(
		final Runnable task, final long delayMillis) {

//		if (DEBUG) Log.v(TAG, "queueEvent:task=" + task + ", delayMillis=" + delayMillis);
		if ((task == null) || (mAsyncHandler == null)) return;
		try {
			mAsyncHandler.removeCallbacks(task);
			if (delayMillis > 0) {
				mAsyncHandler.postDelayed(task, delayMillis);
			} else if (mWorkerThreadID == Thread.currentThread().getId()) {
				task.run();
			} else {
				mAsyncHandler.post(task);
			}
		} catch (final Exception e) {
			// ignore
		}
	}

	/**
	 * 指定したRunnableをワーカースレッド上で実行予定であればキャンセルする
	 * @param task
	 */
	protected final synchronized void removeEvent(final Runnable task) {
		if (task == null) return;
		try {
			mAsyncHandler.removeCallbacks(task);
		} catch (final Exception e) {
			// ignore
		}
	}

	private Snackbar mSnackbar;
	private synchronized void showMessage(final String message, final int duration) {
		if (DEBUG) Log.v(TAG, "showMessage:");
		cancelMessage();
		mSnackbar = Snackbar.make(mRootView, message, duration);
		mSnackbar.show();
	}

	private synchronized void cancelMessage() {
		if (DEBUG) Log.v(TAG, "cancelMessage:");
		if (mSnackbar != null) {
			mSnackbar.dismiss();
			mSnackbar = null;
		}
	}
	
	private void setKeepScreenOn(final boolean onoff) {
		runOnUiThread(() -> {
			final Window window = getWindow();
			if (onoff) {
				window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			} else {
				window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
			}
		});
	}

//================================================================================
	@SuppressLint("NewApi")
	@Override
	public void onMessageDialogResult(final MessageDialogFragmentV4 dialog,
		final int requestCode, final String[] permissions, final boolean result) {

		if (DEBUG) Log.v(TAG, "onMessageDialogResult:");
		if (result) {
			// メッセージダイアログでOKを押された時はパーミッション要求する
			if (BuildCheck.isMarshmallow()) {
				requestPermissions(permissions, requestCode);
				return;
			}
		}
		// メッセージダイアログでキャンセルされた時とAndroid6でない時は
		// 自前でチェックして#checkPermissionResultを呼び出す
		for (final String permission: permissions) {
			checkPermissionResult(requestCode, permission,
				PermissionCheck.hasPermission(this, permission));
		}
	}

	/**
	 * パーミッション要求の結果をチェック
	 * ここではパーミッションを取得できなかった時にToastでメッセージ表示するだけ
	 * @param requestCode
	 * @param permission
	 * @param result
	 */
	protected void checkPermissionResult(
		final int requestCode, final String permission, final boolean result) {
		
		if (DEBUG) Log.v(TAG, "checkPermissionResult:" + permission + ",result=" + result);
		// パーミッションがないときにはメッセージを表示する FIXME Toastの代わりにshowMessageを使う
		if (!result && (permission != null)) {
			if (Manifest.permission.RECORD_AUDIO.equals(permission)) {
				Toast.makeText(this, R.string.permission_audio, Toast.LENGTH_SHORT).show();
			}
			if (Manifest.permission.WRITE_EXTERNAL_STORAGE.equals(permission)) {
				Toast.makeText(this, R.string.permission_ext_storage, Toast.LENGTH_SHORT).show();
			}
			if (Manifest.permission.INTERNET.equals(permission)) {
				Toast.makeText(this, R.string.permission_network, Toast.LENGTH_SHORT).show();
			}
		}
	}

//================================================================================
	private void initView() {
		// Example of a call to a native method
		TextView tv = (TextView) findViewById(R.id.sample_text);
		tv.setText("Hello world");
		mRootView = findViewById(R.id.activity);
	}

	private void close(final boolean canFinish) {
		if (DEBUG) Log.v(TAG, "close:" + canFinish);
		// FIXME 未実装
	}

	/**
	 * カメラダイアログが表示されていれば更新する
	 * @return カメラダイアログが表示されているかどうか falseなら表示されていない
	 */
	private boolean updateCameraDialog() {
		if (DEBUG) Log.v(TAG, "updateCameraDialog:");
		final Fragment fragment = getSupportFragmentManager().findFragmentByTag("CameraDialog");
		if (fragment instanceof CameraDialogV4) {
			((CameraDialogV4)fragment).updateDevices();
			return true;
		}
		return false;
	}

	/**
	 * 接続されているカメラを確認する
	 * 1台も接続されていない時は画面にメッセージを表示する
	 */
	protected void updateConnectedCameras() {
		if (DEBUG) Log.v(TAG, "updateConnectedCameras:");
		queueEvent(new Runnable() {
			@Override
			public void run() {
				final int n = (mUSBMonitor != null) ? mUSBMonitor.getDeviceCount() : 0;
				showMessage("カメラが見つかりません", Snackbar.LENGTH_INDEFINITE);
			}
		}, 0);
	}

	private int mCameraState = CAMERA_NON;

	/**
	 * 接続されているUVCカメラが1台だけでパーミッションが有れば開く
	 */
	private void tryOpenDevice(final boolean requestPermission, final long delayMillis) {
		if (DEBUG) Log.v(TAG, "tryOpenDevice:mCameraState=" + mCameraState);
		if (!mUSBMonitor.isRegistered()) return;
//		mCameraHandler.tryOpen(requestPermission, delayMillis);
	}

	private final USBMonitor.OnDeviceConnectListener
		mOnDeviceConnectListener = new USBMonitor.OnDeviceConnectListener() {

		@Override
		public void onAttach(final UsbDevice device) {
			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onAttach:");
			synchronized (mSync) {
				if (!updateCameraDialog()) {	// カメラダイアログを更新する(カメラダイアログが表示されて無ければfalseが返る)
					if (mCameraState == CAMERA_NON) {
						// カメラダイアログが表示されてなくてカメラがopenしていなければopenを試みる
						tryOpenDevice(true, 0);	// true:接続されているカメラが1台だけならパーミッションを要求してopenを試みる
					}
				}
			}	// synchronized (mCameraSync)
		}

		@Override
		public void onConnect(final UsbDevice device,
			final USBMonitor.UsbControlBlock ctrlBlock, final boolean createNew) {

			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onConnect:");
			boolean openDevice = false;
			synchronized (mSync) {
				if (mCameraState == CAMERA_TRYOPEN) {
					mCameraState = CAMERA_INITIALIZING;
					openDevice = true;
				}
			}	// synchronized (mCameraSync)
			if (openDevice) {
				// カメラをopen
				try {
				} catch (final Exception e) {
					synchronized (mSync) {
						mCameraState = CAMERA_NON;
					}	// synchronized (mCameraSync)
					return;
				}
//				mCameraHandler.open(ctrlBlock);
			}
		}

		@Override
		public void onDisconnect(final UsbDevice device,
			final USBMonitor.UsbControlBlock ctrlBlock) {

			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onDisconnect:");
			synchronized (mSync) {
//				if ((mCameraState != CAMERA_NON) && mCamera.isEqual(device)) {
//					mCameraHandler.close(true);
//				}
			}	// synchronized (mCameraSync)
		}

		@Override
		public void onDettach(final UsbDevice device) {
			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onDettach:");
			updateConnectedCameras();
			updateCameraDialog();
		}

		@Override
		public void onCancel(final UsbDevice device) {
			if (DEBUG) Log.v(TAG, "onCancel:");
			// パーミッションを取得できなかった時
			synchronized (mSync) {
				mCameraState = CAMERA_NON;
			}
			updateConnectedCameras();
		}
	};
}