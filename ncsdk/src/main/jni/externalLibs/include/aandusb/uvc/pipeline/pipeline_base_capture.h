/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_BASE_CAPTURE_H
#define PIPELINE_BASE_CAPTURE_H

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
// uvc/pipeline
#include "pipeline_base_buffered.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

/**
 * 上位パイプラインから受け取ったフレームを最新の1フレームだけを保持して
 * 別スレッド上で処理するためのベースクラス
 */
class CaptureBasePipeline : virtual public AbstractBufferedPipeline {
private:
	static void *capture_thread_func(void *vptr_args);
	void internal_do_capture(JNIEnv *env);
protected:
	volatile bool mIsCapturing;
	mutable Mutex capture_mutex;
	Condition capture_sync;
	pthread_t capture_thread;
	VideoFrame *captureQueue;			// keep latest one frame only
	uint32_t frameWidth;
	uint32_t frameHeight;

	CaptureBasePipeline(const size_t &_data_bytes = DEFAULT_FRAME_SZ);
	CaptureBasePipeline(
		const int &_max_buffer_num = DEFAULT_MAX_FRAME_NUM,
		const int &init_pool_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ);

	virtual ~CaptureBasePipeline();

	void clear_capture_frame();
	void add_capture_frame(VideoFrame *frame);
	VideoFrame *wait_capture_frame();
	// AbstractBufferedPipeline
	virtual void on_start();
	virtual void on_stop();
	virtual int handle_frame(VideoFrame *frame);
	virtual void do_capture(JNIEnv *env) = 0;
public:
	const bool is_capturing() const;
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_BASE_CAPTURE_H
