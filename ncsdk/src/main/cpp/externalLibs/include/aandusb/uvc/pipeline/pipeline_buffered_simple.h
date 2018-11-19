/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_BUFFERED_SIMPLE_H
#define PIPELINE_BUFFERED_SIMPLE_H

// uvc/pipeline
#include "pipeline_base_buffered.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

class SimpleBufferedPipeline : virtual public AbstractBufferedPipeline {
protected:
	// AbstractBufferedPipelineの純粋仮想関数
	/** フレーム処理ループ開始時の処理, フレーム処理スレッド上で実行される */
	virtual void on_start();
	/** フレーム処理ループ終了時の処理, フレーム処理スレッド上で実行される */
	virtual void on_stop();
	/* フレーム処理, フレーム処理スレッド上で実行される
	 * 0なら継続して下位パイプラインへ引き渡す,
	 * 0以外なら下位パイプラインへ渡さない
	 * この関数内ではrecycle_frameを呼ばないこと
	 */
	virtual int handle_frame(VideoFrame *frame);
public:
	SimpleBufferedPipeline(
		const int &_max_buffer_num = DEFAULT_MAX_FRAME_NUM,
		const int &init_pool_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const bool &drop_frames_when_buffer_empty = true);

	virtual ~SimpleBufferedPipeline();
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_BUFFERED_SIMPLE_H
