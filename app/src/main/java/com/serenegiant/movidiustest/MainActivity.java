package com.serenegiant.movidiustest;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.hardware.usb.UsbDevice;
import android.os.Environment;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.Snackbar;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.serenegiant.dialog.MessageDialogFragmentV4;
import com.serenegiant.ncsdk.IDataLink;
import com.serenegiant.ncsdk.MvNcAPI;
import com.serenegiant.ncsdk.UsbDataLink;
import com.serenegiant.usb.DeviceFilter;
import com.serenegiant.usb.USBMonitor;
import com.serenegiant.utils.BuildCheck;
import com.serenegiant.utils.HandlerThreadHandler;
import com.serenegiant.utils.PermissionCheck;
import com.serenegiant.uvc.CameraDialogV4;
import com.serenegiant.uvc.ICameraDialogListener;

import java.io.File;
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

	private View mRootView;
	private USBMonitor mUSBMonitor;
	private MvNcAPI mMvNcAPI;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		mAsyncHandler = HandlerThreadHandler.createHandler("");
		mWorkerThreadID = mAsyncHandler.getLooper().getThread().getId();
		mMvNcAPI = new MvNcAPI(this);
		initView();
		if (mUSBMonitor == null) {
			mUSBMonitor = new USBMonitor(this, mOnDeviceConnectListener);
			final List<DeviceFilter> filters
				= DeviceFilter.getDeviceFilters(this, R.xml.device_filter);
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
		close();
		if (mMvNcAPI != null) {
			mMvNcAPI.release();
			mMvNcAPI = null;
		}
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
		if (DEBUG) Log.v(TAG, "internalOnResume:" + mUSBMonitor);
		checkPermissionWriteExternalStorage();
		if ((mUSBMonitor != null) && !mUSBMonitor.isRegistered()) {
			if (DEBUG) Log.v(TAG, "internalOnResume:register USBMonitor");
			mUSBMonitor.register();
		}
	}

	protected void internalOnPause() {
		if (DEBUG) Log.v(TAG, "internalOnPause:" + mUSBMonitor);
		try {
			if ((mUSBMonitor != null) && mUSBMonitor.isRegistered()) {
				mUSBMonitor.unregister();
				if (DEBUG) Log.v(TAG, "internalOnResume:unregister USBMonitor");
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
	
	@Override
	public void onRequestPermissionsResult(final int requestCode,
		@NonNull final String[] permissions, @NonNull final int[] grantResults) {

		super.onRequestPermissionsResult(requestCode, permissions, grantResults);
		if (DEBUG) Log.v(TAG, "onRequestPermissionsResult:");
		final int n = Math.min(permissions.length, grantResults.length);
		for (int i = 0; i < n; i++) {
			checkPermissionResult(requestCode, permissions[i],
				grantResults[i] == PackageManager.PERMISSION_GRANTED);
		}
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

		if (DEBUG) Log.v(TAG, "queueEvent:task=" + task + ", delayMillis=" + delayMillis);
		if (task == null) return;
		if (mAsyncHandler == null) {
			Log.w(TAG, "queueEvent:already released?");
			return;
		}
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
			Log.w(TAG, e);
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

	protected static final int REQUEST_PERMISSION_WRITE_EXTERNAL_STORAGE = 0x2345;
	/**
	 * 外部ストレージへの書き込みパーミッションが有るかどうかをチェック
	 * なければ説明ダイアログを表示する
	 * @return true 外部ストレージへの書き込みパーミッションが有る
	 */
	protected boolean checkPermissionWriteExternalStorage() {
		if (DEBUG) Log.v(TAG, "checkPermissionWriteExternalStorage:");
		if (!PermissionCheck.hasWriteExternalStorage(this)) {
			MessageDialogFragmentV4.showDialog(this, REQUEST_PERMISSION_WRITE_EXTERNAL_STORAGE,
				R.string.permission_title, R.string.permission_audio_recording_request,
				new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE});
			return false;
		}
		return true;
	}

//================================================================================
	private void initView() {
		// Example of a call to a native method
		Button button = findViewById(R.id.compute_button);
		button.setOnClickListener(mOnClickListener);
		button = findViewById(R.id.reset_button);
		button.setOnClickListener(mOnClickListener);

		mRootView = findViewById(R.id.activity);
	}

	private final View.OnClickListener mOnClickListener
		= new View.OnClickListener() {
		@Override
		public void onClick(final View v) {
			switch (v.getId()) {
			case R.id.compute_button:
				compute();
				break;
			case R.id.reset_button:
				reset();
				break;
			default:
				break;
			}
		}
	};

	private void compute() {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Log.i(TAG, "start");
				final File dir = new File(
					Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS),
					"MovidiusTest");
				String dataPath = dir.getAbsolutePath();
				if (!dataPath.endsWith("/")) {
					dataPath = dataPath + "/";
				}
				if (DEBUG) Log.v(TAG, "dataPath=" + dataPath);
				try {
					mMvNcAPI.run(dataPath);
				} catch (final Exception e) {
					Log.w(TAG, e);
				}
				Log.i(TAG, "finished!");
			}
		}, TAG).start();
	}

	private void reset() {
		mMvNcAPI.resetAll();
	}

	private void close(@NonNull final IDataLink dataLink) {
		
		mMvNcAPI.remove(dataLink);
		dataLink.release();
		if (mMvNcAPI.getNumDataLinks() == 0) {
			runOnUiThread(() -> {
				finish();
			});
		}
	}

	private void close() {
		if (DEBUG) Log.v(TAG, "close:");
		synchronized (mSync) {
			final List<IDataLink> list = mMvNcAPI.getDatalinkAll();
			if (!list.isEmpty()) {
				mCameraState = CAMERA_NON;
				queueEvent(() -> {
					for (final IDataLink dataLink: list) {
						synchronized (mSync) {
							if (mMvNcAPI != null) {
								mMvNcAPI.remove(dataLink);
							}
						}
						dataLink.release();
					}
					runOnUiThread(() -> {
						finish();
					});
				}, 0);
			} else {
				runOnUiThread(() -> {
					finish();
				});
			}
		}
	}

	/**
	 * Movidiusの選択要求
	 * @param requestPermission
	 */
	protected void handleTryOpen(final boolean requestPermission) {
	    if (DEBUG) Log.v(TAG, "handleTryOpen:");
		synchronized (mSync) {
			if (mCameraState == CAMERA_NON) {
				mCameraState = CAMERA_TRYOPEN;
				final List<UsbDevice> devices = mUSBMonitor.getDeviceList();
				final int n = devices.size();
				if ((n == 1)
					&& (requestPermission || mUSBMonitor.hasPermission(devices.get(0))) ) {
					if (DEBUG) Log.v(TAG, "handleTryOpen:requestPermission");
					// 接続されているUVCカメラが1台だけでパーミッションが有るかパーミッション要求フラグがセットされていれば開く
					final boolean result = mUSBMonitor.requestPermission(devices.get(0));
					if (result) {
						showMessage("USBホスト機能が無効です", Snackbar.LENGTH_INDEFINITE);
					}
				} else {
					if (n > 1) {
						CameraDialogV4.showDialog(this);
						return;
					}
					mCameraState = CAMERA_NON;
					if (n == 0) {
						showMessage("Movidiusが見つかりません", Snackbar.LENGTH_INDEFINITE);
					}
				}
			}
		}
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
		queueEvent(() -> {
			final int n = (mUSBMonitor != null) ? mUSBMonitor.getDeviceCount() : 0;
			showMessage("カメラが見つかりません", Snackbar.LENGTH_INDEFINITE);
		}, 0);
	}

	private int mCameraState = CAMERA_NON;

	/**
	 * 接続されているUVCカメラが1台だけでパーミッションが有れば開く
	 */
	private void tryOpenDevice(final boolean requestPermission, final long delayMillis) {
		if (DEBUG) Log.v(TAG, "tryOpenDevice:mCameraState=" + mCameraState);
		if (!mUSBMonitor.isRegistered()) {
			Log.w(TAG, "unexpectedly USBMonitor is not registered.");
			return;
		}
		queueEvent(() -> {
			handleTryOpen(requestPermission);
		}, delayMillis);
	}

	private final USBMonitor.OnDeviceConnectListener
		mOnDeviceConnectListener = new USBMonitor.OnDeviceConnectListener() {

		@Override
		public void onAttach(final UsbDevice device) {
			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onAttach:");
			cancelMessage();
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
			cancelMessage();
			boolean openDevice = false;
			synchronized (mSync) {
				if (mCameraState == CAMERA_TRYOPEN) {
					mCameraState = CAMERA_INITIALIZING;
					openDevice = true;
				}
			}	// synchronized (mCameraSync)
			if (openDevice) {
				// カメラをopen
				final UsbDataLink dataLink;
				try {
					dataLink = new UsbDataLink(MainActivity.this);
				} catch (final Exception e) {
					synchronized (mSync) {
						mCameraState = CAMERA_NON;
					}	// synchronized (mCameraSync)
					return;
				}
				queueEvent(() -> {
					try {
						dataLink.open(ctrlBlock);
						mMvNcAPI.add(dataLink);
					} catch (final Exception e) {
						Log.w(TAG, e);
					}
				}, 0);
			}
		}

		@Override
		public void onDisconnect(final UsbDevice device,
			final USBMonitor.UsbControlBlock ctrlBlock) {

			if (DEBUG) Log.v(TAG, "OnDeviceConnectListener:onDisconnect:");
			synchronized (mSync) {
				if ((mCameraState != CAMERA_NON)) {
					final IDataLink dataLink = mMvNcAPI.getDatalink(device);
					if (dataLink != null) {
						close(dataLink);
					}

				}
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
