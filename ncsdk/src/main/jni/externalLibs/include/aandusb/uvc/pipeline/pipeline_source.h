/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_PIPELINE_SOURCE_H
#define USBWEBCAMERAPROJ_PIPELINE_SOURCE_H

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
#include "uvc/uvc_frame.h"
// uvc/pipeline
#include "pipeline_base.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

/**
 * Java側のByteBufferのデータを映像ソースとして使うためのパイプライン(始点)
 * バッファリング無し
 */
class PipelineSource : virtual public IPipelineParent {
private:
	VideoFrame mWorkFrame;
protected:
	mutable Mutex pipeline_mutex;
	IPipeline *next_pipeline;
	virtual void on_reset();
public:
	PipelineSource();
	virtual ~PipelineSource();
	virtual int set_pipeline(IPipeline *pipeline);
	virtual int queue_frame(JNIEnv *env, jobject frame_obj,
		const uint32_t &size, const uint32_t &width, const uint32_t &height,
		const nsecs_t &presentation_time_us,
		const raw_frame_t &frame_type, const uint32_t &flags = 0);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_PIPELINE_SOURCE_H
