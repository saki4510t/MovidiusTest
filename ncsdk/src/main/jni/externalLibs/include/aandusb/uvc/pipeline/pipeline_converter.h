/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_CONVERTER_H
#define PIPELINE_CONVERTER_H

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
 * ピクセルフォーマットを変換するためのパイプライン
 * バッファリング有り
 */
class ConvertPipeline : virtual public AbstractBufferedPipeline {
private:
	const raw_frame_t target_frame_type;
protected:
	// AbstractBufferedPipeline
	virtual void on_start();
	virtual void on_stop();
	virtual int handle_frame(VideoFrame *frame);
public:
	ConvertPipeline(const size_t &_data_bytes, const raw_frame_t &target_frame_type = RAW_FRAME_UNKNOWN);
	virtual ~ConvertPipeline();
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_CONVERTER_H
