/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVCCLIENT_H_
#define UVCCLIENT_H_

#include <jni.h>

#include "time_utc.h"
// uvc
#include "uvc/uvc_controller.h"
#include "uvc/uvc_transfer_status.h"	// FIXME このincludeはcppの方へ移動させたいけどUSE_STATUS_TRANSFERマクロがあるのでここでincludeする
// main
#include "jni_uvc_defs.h"
#include "uvc_preview.h"
#include "uvc_status_event_handler.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class VideoFrame;
class UVCPreview;
#if USE_STATUS_TRANSFER
class StatusTransfer;
class UVCStatusEventHandler;
#endif

/**
*
*/
class UVCClient : virtual public UVCController {
private:
	jobject uvcclient_obj;
	char *mDataDir;
	// プレビュー用
	UVCPreview *mPreview;
	char *captureRootDir;
	AAssetManager *mAssetManager;
#if USE_STATUS_TRANSFER
	StatusTransfer *status_transfer;
	UVCStatusEventHandler event_handler;
#endif
protected:
	static void frame_callback(VideoFrame *frame, void *usr_args);
public:
	UVCClient(jobject uvccamera_obj, AAssetManager *assetmanager, char *cache_dir, const TIME_T expiration);
	virtual ~UVCClient();
	virtual void release(JNIEnv *env);

	int connect(JNIEnv *env, const int &fd, const bool &use_status_callback);
	int disconnect();
	// JSON形式でカメラの現在のストリーム設定を返す
	// 外部ストリームが有効ならば外部ストリーム設定を取得
	// 外部ストリーム設定が無効ならばメインストリームを取得
	char *get_current_stream();
	// キャプチャ関連
	int set_capture_display(ANativeWindow *preview_window, const int &captureFormat);
	int set_encoder(VideoEncoder *videoEncoder);
	// プレビュー関係
	int set_preview_size(const previewmode_t &mode,
		const uint32_t &request_width, const uint32_t &request_height,
		const float &request_min_fps, const float &request_max_fps,
		const float &bandwidth_factor = 0.0f);
	int set_preview_display(ANativeWindow *preview_window, const bool &low_reso);
	int start_preview();
	int stop_preview();
	int take_picture(const char *path,
		const uint32_t &width, const uint32_t &height,
		const int &compress = DEFAULT_JPEG_COMPRESS,
		jobject capture_callback_obj = NULL);
	int set_max_fps(const int &maxFps);

	int set_status_callback(JNIEnv *env, jobject status_callback_obj);
	int set_button_callback(JNIEnv *env, jobject button_callback_obj);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVCCLIENT_H_ */
