/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_PIPELINE_CALLBACK_NATIVE_H
#define USBWEBCAMERAPROJ_PIPELINE_CALLBACK_NATIVE_H

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
#include "uvc/uvc_frame.h"
#include "uvc/uvc_frame_pool.h"
// uvc/pipeline
#include "pipeline_base.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

typedef int (*pipeline_on_start_callback_t)(JNIEnv *env, void *frame_callback_args);
typedef int (*pipeline_on_stop_callback_t)(JNIEnv *env, void *frame_callback_args);
typedef int (*pipeline_on_frame_callback_t)(JNIEnv *env, void *frame_callback_args, VideoFrame *frame);

/**
 * native側でパイプライン処理を拡張できるようにするためのパイプライン
 * バッファリング有り
 */
class NativeCallbackPipeline : virtual public UVCFramePool, virtual public IPipeline {
private:
	void *callback_args;
	pipeline_on_start_callback_t on_start_callback_func;
	pipeline_on_stop_callback_t on_stop_callback_func;
	pipeline_on_frame_callback_t on_frame_callback_func;
	pthread_t handler_thread;	// フレーム処理用のスレッド
	mutable Mutex frame_mutex;
	Condition frame_sync;
	VideoFrame *queued_frame;
	static void *handler_thread_func(void *vptr_args);	// フレーム処理用スレッドの実行関数
protected:
	VideoFrame *wait_frame();
	void clear_frame();
	void do_loop(JNIEnv *env);
public:
	NativeCallbackPipeline(void *callback_args = NULL,
		pipeline_on_frame_callback_t on_frame_callback = NULL,
		pipeline_on_start_callback_t on_start_callback = NULL,
		pipeline_on_stop_callback_t on_stop_callback = NULL);

	virtual ~NativeCallbackPipeline();

	virtual int release();
	virtual int start();
	virtual int stop();
	virtual int queue_frame(VideoFrame *frame);

	int set_callback(void *callback_args,
		pipeline_on_frame_callback_t on_frame_callback,
		pipeline_on_start_callback_t on_start_callback,
		pipeline_on_stop_callback_t on_stop_callback);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_PIPELINE_CALLBACK_NATIVE_H
