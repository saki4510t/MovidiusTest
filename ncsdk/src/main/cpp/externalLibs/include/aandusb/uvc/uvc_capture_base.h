/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef AANDUSB_UVC_CAPTURE_BASE_H
#define AANDUSB_UVC_CAPTURE_BASE_H

#include <jni.h>
#include <vector>
#include <pthread.h>
#include <queue>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include "assets.h"
// core
#include "core/internal.h"
// uvc
#include "aanduvc.h"
#include "uvc_frame_pool.h"

#include "VideoEncoder.h"

namespace serenegiant {
namespace usb {
namespace uvc {

#pragma interface

// デフォルトのjpeg圧縮率[%]
#define DEFAULT_JPEG_COMPRESS 80

class UVCCaptureBase : virtual public UVCFramePool {
private:
	Assets *mAssets;
	VideoEncoder *mVideoEncoder;
	ANativeWindow *mCaptureWindow;
	volatile bool mIsRunning;			// キャプチャスレッド実行中
	volatile bool mIsCapturing;			// 動画撮影中
	int mCaptureFormat;					// キャプチャ用のANativeWindowの表示フォーマット

	VideoFrame *captureQueue;			// 最新の1フレームだけを保持可能に変更
	mutable Mutex capture_mutex;		// ミューテックス
	Condition capture_sync;				// 同期オブジェクト
	mutable Mutex capture_queue_mutex;	// ミューテックス(キャプチャキュー同期用)
	Condition capture_queue_sync;		// 同期オブジェクト(キャプチャキュー同期用)

	volatile bool request_still_capture;
	volatile still_capture_method_t stillCaptureMethod;
	char *picturePath;					// 撮影した写真の保存パス
	jobject capture_callback_obj;		// キャプチャーコールバックはグローバル参照
	int compress;						// jpeg圧縮率[%], pngの時は無効

	virtual void check_take_picture(JNIEnv *env, VideoFrame *frame);
	// キャプチャフレームキューからフレームを取得、キューが空の時はブロックする
	VideoFrame *wait_capture_frame();
	// キャプチャフレームキューからフレームを取得、キューが空の時はNULLを返す
	VideoFrame *peek_capture_frame();

	pthread_t capture_thread;			// キャプチャスレッド
//======================================================================
	void clear_capture_display();
//======================================================================
	// キャプチャスレッドの実行関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static void *capture_thread_func(void *vptr_args);
	// キャプチャスレッドの実行関数
	void do_capture();
	// キャプチャスレッドの実体(Surfaceへ書き込む時)
	void do_capture_surface();
	// キャプチャスレッドの実体(エンコーダーへ書き込む時)
	void do_capture_encoder();
	// キャプチャスレッドの実体(静止画キャプチャのみ)
	void do_capture_picture(JNIEnv *env);
protected:
	UVCCaptureBase(Assets *assets);
	virtual ~UVCCaptureBase();

	virtual int start_capture();
	virtual int stop_capture(const bool &need_join = false);
	virtual int resize_capture(const uint32_t &width, const uint32_t &height);
	virtual int after_stop_capture();
	inline const still_capture_method_t current_still_capture_method() const { return stillCaptureMethod; };
	// キャプチャフレームキューに追加
	void add_capture_frame(VideoFrame *frame);
	virtual const uint32_t get_width() const = 0;
	virtual const uint32_t get_height() const = 0;
	virtual still_capture_method_t request_take_picture(const uint32_t &width,
														const uint32_t &height) = 0;
	virtual void handle_capture(VideoFrame *frame);
	// キャプチャフレームキューをクリア
	void clear_capture_frame();
public:
	// 動画キャプチャ用のSurfaceをセット
	inline const bool is_capturing() const { return mIsCapturing; };
	inline const bool is_still_capturing() const { return request_still_capture; };
	int set_capture_display(ANativeWindow *capture_window, const int &captureFormat);
	int set_encoder(VideoEncoder *videoEncoder);
	// 静止画をキャプチャ
	int take_picture(const char *path,
		const uint32_t &width, const uint32_t &height,
		const int &compress = DEFAULT_JPEG_COMPRESS,
		jobject capture_callback_obj = NULL);	// キャプチャーコールバックはグローバル参照
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //AANDUSB_UVC_CAPTURE_BASE_H
