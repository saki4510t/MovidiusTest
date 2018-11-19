/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PUPILMOBILE_IPIPELINE_H
#define PUPILMOBILE_IPIPELINE_H

#include <stdlib.h>
#include <pthread.h>

// core
#include "core/internal.h"

#include "uvc/aanduvc.h"
#include "uvc/uvc_frame.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

#define DEFAULT_FRAME_SZ 1024

typedef enum pipeline_type {
	PIPELINE_TYPE_SIMPLE_BUFFERED = 0,
	PIPELINE_TYPE_UVC_CONTROL = 100,
	PIPELINE_TYPE_CALLBACK = 200,
	PIPELINE_TYPE_CALLBACK_NATIVE = 300,
	PIPELINE_TYPE_CONVERT = 400,
	PIPELINE_TYPE_PREVIEW = 500,
	PIPELINE_TYPE_DISTRIBUTE = 600,
	PIPELINE_TYPE_PTS_CALC = 700,
} pipeline_type_t;

typedef enum _pipeline_state {
	PIPELINE_STATE_UNINITIALIZED = 0,
	PIPELINE_STATE_RELEASING = 10,
	PIPELINE_STATE_INITIALIZED = 20,
	PIPELINE_STATE_STARTING = 30,
	PIPELINE_STATE_RUNNING = 40,
	PIPELINE_STATE_STOPPING = 50,
} pipeline_state_t;

class IPipeline;

class IPipelineParent {
friend IPipeline;
private:
	IPipelineParent *_parent;
protected:
	mutable Mutex parent_mutex;
	// FIXME これもpackets_per_transferと同じで、自前で親をたどってPreviewPipelineを探すようにしたほうがいいかも
	virtual still_capture_method_t request_take_picture(
		const uint32_t &width, const uint32_t &height);
public:
	IPipelineParent();
	virtual IPipelineParent *setParent(IPipelineParent *parent);
	virtual IPipelineParent *getParent();
};

class DistributePipeline;
class PipelineSource;

class IPipeline : virtual public IPipelineParent {
friend DistributePipeline;
friend PipelineSource;
private:
	volatile bool mIsRunning;
	volatile pipeline_state_t state;
	// force inhibiting copy/assignment
	IPipeline(const IPipeline &src);
	void operator =(const IPipeline &src);
protected:
	const size_t default_frame_size;
	mutable Mutex pipeline_mutex;
	IPipeline *next_pipeline;

	IPipeline(const size_t &default_frame_size = DEFAULT_FRAME_SZ);
	virtual ~IPipeline();
	void set_state(const pipeline_state_t &new_state);
	inline bool set_running(const bool &is_running) {
		const bool prev = mIsRunning;
		mIsRunning  = is_running;
		return (prev);
	};
	int chain_frame(VideoFrame *frame);
	/**
	 * Called from upstream when pipeline is reset while running.
	 */
	virtual void on_reset();
	/**
	 * Called from upstream when still image is captured.
	 */
	virtual void on_capture(VideoFrame *frame);
public:
	const pipeline_state_t get_state() const;
	inline const bool is_running() const { return mIsRunning; };
	virtual int set_pipeline(IPipeline *pipeline);
	virtual int release() = 0;
	virtual int start() = 0;
	virtual int stop() = 0;
	virtual int queue_frame(VideoFrame *frame);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PUPILMOBILE_IPIPELINE_H
