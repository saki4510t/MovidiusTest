/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_BASE_BUFFERED_H
#define PIPELINE_BASE_BUFFERED_H

#include <stdlib.h>
#include <pthread.h>
#include <list>

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
#include "uvc/uvc_frame_pool.h"
// uvc/pipeline
#include "pipeline_base.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

class AbstractBufferedPipeline;

/**
 * フレームのバッファリングを行い非同期で次のパイプラインへ引き渡すためのベースクラス
 */
class AbstractBufferedPipeline : virtual public UVCFramePool, virtual public IPipeline {
private:
	/** フレームバッファで保持できる最大数 */
	const uint32_t max_buffer_num;
	/** フレームバッファの初期数 */
	const uint32_t init_pool_num;

	pthread_t handler_thread;	// フレーム処理用のスレッド
	mutable Mutex buffer_mutex;
	Condition buffer_sync;
	std::list<VideoFrame *> frame_queue;	// FIFOにしないといけないのでstd::listを使う
	static void *handler_thread_func(void *vptr_args);	// フレーム処理用スレッドの実行関数

protected:
	AbstractBufferedPipeline(
		const uint32_t &_max_buffer_num = DEFAULT_MAX_FRAME_NUM,
		const uint32_t &init_pool_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &default_frame_size = DEFAULT_FRAME_SZ,
		const bool &drop_frames_when_buffer_empty = true);

	virtual ~AbstractBufferedPipeline();

	/** フレームキューをクリア */
	void clear_frames();
	/** フレームキューに追加 */
	int add_frame(VideoFrame *frame);
	/** フレームキューからフレームを取り出す。空なら待機する */
	VideoFrame *wait_frame();
	/** フレームキュー中のフレーム数を取得する */
	uint32_t get_frame_count();
	/** フレーム処理ループ */
	virtual void do_loop();
	/** フレーム処理ループ開始時の処理, フレーム処理スレッド上で実行される */
	virtual void on_start() = 0;
	/** フレーム処理ループ終了時の処理, フレーム処理スレッド上で実行される */
	virtual void on_stop() = 0;
	/* フレーム処理, フレーム処理スレッド上で実行される
	 * 0なら継続して下位パイプラインへ引き渡す,
	 * 0以外なら下位パイプラインへ渡さない
	 * この関数内ではrecycle_frameを呼ばないこと
	 */
	virtual int handle_frame(VideoFrame *frame) = 0;
public:
	virtual int release();
	virtual int start();
	virtual int stop();
	virtual int queue_frame(VideoFrame *frame);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_BASE_BUFFERED_H
