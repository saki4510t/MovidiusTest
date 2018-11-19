/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_DISTRIBUTOR_H
#define PIPELINE_DISTRIBUTOR_H

// uvc/pipeline
#include "pipeline_base_buffered.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

/**
 * 受け取った映像を複数のパイプラインへ分配するためのパイプライン
 * バッファリング有り
 */
class DistributePipeline : virtual public AbstractBufferedPipeline {
private:
	std::list<IPipeline *> pipelines;
protected:
	// AbstractBufferedPipeline
	virtual void on_start();
	virtual void on_stop();
	virtual int handle_frame(VideoFrame *frame);
	// IPipeline
	virtual void on_capture(VideoFrame *frame);
public:
	DistributePipeline(
		const int &_max_buffer_num = DEFAULT_MAX_FRAME_NUM,
		const int &init_pool_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const bool &drop_frames_when_buffer_empty = true);

	virtual ~DistributePipeline();
	virtual int add_pipeline(IPipeline *pipeline);
	virtual int remove_pipeline(IPipeline *pipeline);
	virtual const int get_counts();
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_DISTRIBUTOR_H
