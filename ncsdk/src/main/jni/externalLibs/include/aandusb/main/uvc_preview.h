/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVCPREVIEW_H_
#define UVCPREVIEW_H_

#include <jni.h>
#include <vector>
#include <pthread.h>

#include "VideoEncoder.h"
// uvc
#include "uvc/uvc_camera_base.h"
#include "uvc/uvc_surface_preview.h"
#include "uvc/uvc_capture_base.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {

/**
*
*/
class UVCPreview :
	virtual public UVCCameraBase,
	virtual public UVCSurfacePreviewBase,
	virtual public UVCCaptureBase {
private:
	// Java側のオブジェクトへのグローバル参照(参照自体はUVCClientで管理しているので破棄はしなくてOK)
	jobject uvcclient_obj;
	Fields_iframecallback iframecallback_fields;

	// 静止画撮影時のコールバック関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static int still_capture_frame_callback(VideoFrame *frame, void *vptr_args);
protected:
//--------------------------------------------------------------------------------
	// UVCCameraBase
	virtual void do_stream(JNIEnv *env);
	virtual void on_frame_callback(VideoFrame *frame);
	virtual void on_start_streaming();
	virtual void on_stop_streaming();
	virtual void after_stop_stream();
//--------------------------------------------------------------------------------
	// UVCSurfacePreviewBase
	int setup_preview_display();
	virtual void on_start_preview(JNIEnv *env);
	virtual void on_stop_preview(JNIEnv *env);
	virtual void on_resize_preview(JNIEnv *env);
	virtual bool on_handle_frame(JNIEnv *env, VideoFrame *frame);
//--------------------------------------------------------------------------------
	// UVCCaptureBase
	virtual const uint32_t get_width() const;
	virtual const uint32_t get_height() const;
	virtual still_capture_method_t request_take_picture(const uint32_t &width, const uint32_t &height);
public:
	UVCPreview(JNIEnv *env,
		jobject _uvcclient_obj,
		Assets *assets, Device *device,
		const uint32_t &num_transfer_bufs = NUM_TRANSFER_BUFS,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE,
		const bool &enable_capture_method = false);
	virtual ~UVCPreview();

	// プレビュー関係
	virtual const bool is_running() const;
	virtual int start_stream(const int &_cull_steps = 0);
	virtual int stop_stream();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVCPREVIEW_H_ */
