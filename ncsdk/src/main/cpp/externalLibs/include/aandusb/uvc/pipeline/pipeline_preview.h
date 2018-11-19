/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_PREVIEW_H
#define PIPELINE_PREVIEW_H

// uvc
#include "uvc/uvc_surface_preview.h"
#include "uvc/uvc_capture_base.h"
// uvc/pipeline
#include "pipeline_base.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

/**
 * 受け取った映像を描画するためのパイプライン
 * バッファリング有り
 */
class PreviewPipeline :
	virtual public IPipeline,
	virtual public UVCSurfacePreviewBase,
	virtual public UVCCaptureBase {
private:
	jobject pipeline_obj;
	Fields_iframecallback iframecallback_fields;

	previewmode_t requestPreviewMode;
	uint32_t requestWidth, requestHeight;

	size_t mRawFrameBytes;					// 生フレームデータのサイズ(yuv)
protected:
//--------------------------------------------------------------------------------
	// UVCSurfacePreviewBaseの純粋仮想関数
	virtual void after_do_preview(VideoFrame *frame);
	virtual void on_start_preview(JNIEnv *env);
	virtual void on_stop_preview(JNIEnv *env);
	virtual void on_resize_preview(JNIEnv *env);
	virtual bool on_handle_frame(JNIEnv *env, VideoFrame *frame);
	virtual int setup_preview_display();
//--------------------------------------------------------------------------------
	// UVCCaptureBaseの純粋仮想関数
	virtual const uint32_t get_width() const;
	virtual const uint32_t get_height() const;
	virtual still_capture_method_t request_take_picture(const uint32_t &width, const uint32_t &height);
	virtual void on_capture(VideoFrame *frame);	// IPipeline&UVCSurfacePreviewBaseをoverride
public:
	PreviewPipeline(JNIEnv *env,
		Assets *assets, jobject pipeline_obj,
		const size_t &_data_bytes = DEFAULT_FRAME_SZ);
	virtual ~PreviewPipeline();
	void release(JNIEnv *env);

	// IPipelineの純粋仮想関数
	virtual int release();	// Javaのグローバル参照を開放しないと行けないのでrelease(JNIEnv *env)を呼ぶこと
	virtual int start();
	virtual int stop();
	virtual int queue_frame(VideoFrame *frame);

	int set_preview_size(const previewmode_t &mode, const uint32_t &width, const uint32_t &height);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_PREVIEW_H
