/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_UVC_CLIENT_H_
#define PIPELINE_UVC_CLIENT_H_

#include <jni.h>
#include <vector>
#include <queue>
#include <pthread.h>
#include <android/native_window.h>

// core
#include "core/internal.h"
// uvc
#include "uvc/uvc_camera_base.h"
// uvc/pipeline
#include "pipeline_base_buffered.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

#define MAX_FRAME_PREVIEW 4
#define FRAME_POOL_SZ MAX_FRAME_PREVIEW + 2

/**
 * UVCカメラからの映像をソースとして使うためのパイプライン(始点)
 * バッファリング有り
 */
class UVCClientPipeline
	: virtual public UVCCameraBase,
	  virtual public AbstractBufferedPipeline {
private:
	const int sanitary_check;	// 未使用
	// 静止画撮影時のコールバック関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static int still_capture_frame_callback(VideoFrame *frame, void *vptr_args);
protected:
	// AbstractBufferedPipeline
	virtual void on_start();
	virtual void on_stop();
	virtual int handle_frame(VideoFrame *frame);
	// UVCCameraBaseの純粋仮想関数
	virtual void do_stream(JNIEnv *env);
	virtual void on_frame_callback(VideoFrame *frame);
	virtual void on_start_streaming();
	virtual void on_stop_streaming();
	virtual void after_stop_stream();
	// IPipelineParent
	virtual still_capture_method_t request_take_picture(
		const uint32_t &width, const uint32_t &height);
public:
	UVCClientPipeline(Device *device,
		const uint32_t &_max_buffer_num = FRAME_POOL_SZ,
		const uint32_t &init_pool_num = MAX_FRAME_PREVIEW,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const int &_sanitary_check = 0,
		const int &num_transfer_bufs = NUM_TRANSFER_BUFS,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE,
  		const bool &enable_capture_method = false);

	virtual ~UVCClientPipeline();
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* PIPELINE_UVC_CLIENT_H_ */
