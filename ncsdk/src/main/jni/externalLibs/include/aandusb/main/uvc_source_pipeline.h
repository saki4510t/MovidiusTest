/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_SOURCE_PIPELINE_H
#define UVC_SOURCE_PIPELINE_H

#include <jni.h>
#include "time_utc.h"
#include "assets.h"
#include "VideoEncoder.h"

// core
#include "core/internal.h"
#include "core/context.h"
// uvc
#include "uvc/aanduvc.h"
#include "uvc/uvc_descriptor.h"
#include "uvc/pipeline/pipeline_uvc_client.h"
#include "uvc/uvc_controller.h"
#include "uvc/uvc_transfer_status.h"	// FIXME このincludeはcppの方へ移動させたいけどUSE_STATUS_TRANSFERマクロがあるのでここでincludeする
// main
#include "uvc_status_event_handler.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class UVCDevice;
class StatusTransfer;
class VideoFrame;
class UVCPreview;
#if USE_STATUS_TRANSFER
class StatusTransfer;
class UVCStatusEventHandler;
#endif

namespace pipeline {

/**
*
*/
class UVCSourcePipeline : virtual public UVCController {
private:
	jobject uvcpipeline_obj;
	char *mDataDir;
	// UVCカメラアクセス用
	UVCClientPipeline *camera;
	char *captureRootDir;
	Assets *mAssets;
#if USE_STATUS_TRANSFER
	StatusTransfer *status_transfer;
	UVCStatusEventHandler event_handler;
#endif
protected:
	static void frame_callback(VideoFrame *frame, void *usr_args);
public:
	UVCSourcePipeline(jobject uvccamera_obj, AAssetManager *assetmanager,
		char *cache_dir, const TIME_T &expiration);
	virtual ~UVCSourcePipeline();
	void release(JNIEnv *env);

	const pipeline_state_t get_state() const;
	virtual int set_pipeline(IPipeline *pipeline);

	int connect(const char *dev_name,
		const int &vid, const int &pid, const int &fd,
		const uint32_t &max_buffer_num = FRAME_POOL_SZ,
		const uint32_t &init_pool_num = MAX_FRAME_PREVIEW,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const int &_sanitary_check = 0,
		const int &num_transfer_bufs = NUM_TRANSFER_BUFS,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE,
  		const bool &enable_capture_method = false,
		const bool &use_status_callback = false);

	inline int connect(const char *dev_name,
		const int &vid, const int &pid, const int &fd,
		const uint32_t &max_buffer_num = FRAME_POOL_SZ,
		const uint32_t &init_pool_num = MAX_FRAME_PREVIEW,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const bool &use_status_callback = false) {

		return connect(dev_name,
			vid, pid, fd,
			max_buffer_num, init_pool_num, default_frame_size,
			0, NUM_TRANSFER_BUFS, DEFAULT_DCT_MODE, false, use_status_callback);
	}

	inline int connect(const char *dev_name,
		const int &vid, const int &pid, const int &fd,
		const bool &use_status_callback = false) {

		return connect(dev_name,
			vid, pid, fd,
			MAX_FRAME_PREVIEW, MAX_FRAME_PREVIEW, DEFAULT_FRAME_SZ,
			0, NUM_TRANSFER_BUFS, DEFAULT_DCT_MODE, false, use_status_callback);
	}

	int disconnect();
	// JSON形式でカメラの現在のストリーム設定を返す
	// 外部ストリームが有効ならば外部ストリーム設定を取得
	// 外部ストリーム設定が無効ならばメインストリームを取得
	char *get_current_stream();
	// プレビュー関係
	int set_preview_size(const previewmode_t &mode,
		const uint32_t &request_width, const uint32_t &request_height,
		const float &request_min_fps, const float &request_max_fps,
		const float &bandwidth_factor = 0.0f);
	int set_preview_display(ANativeWindow *preview_window, const bool &low_reso);
	int start_preview();
	int stop_preview();
	// キャプチャ関連
	int take_picture(const char *path,
		const uint32_t &width, const uint32_t &height,
		jobject capture_callback_obj);
	int set_capture_display(ANativeWindow *preview_window, const int &captureFormat);
	int set_encoder(VideoEncoder *video_encoder);

	int set_status_callback(JNIEnv *env, jobject status_callback_obj);
	int set_button_callback(JNIEnv *env, jobject button_callback_obj);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_SOURCE_PIPELINE_H */
