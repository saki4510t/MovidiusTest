/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef AANDUSB_PIPELINE_PTS_CALC_H
#define AANDUSB_PIPELINE_PTS_CALC_H

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
// uvc/pipeline
#include "pipeline_base.h"
#include "pipeline_base_buffered.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

// カメラ側のクロックのばらつき？補正を行う場合(今の補正コードはおかしいから無効にすること)
#define ENABLE_CLOCK_CORRECTION 1
// 計算したpresentationtime_usがモノトニックかどうかをチェックするかどうか
#define CHECK_MONOTONIC 1
// 差分とそのヒストグラムの計算＆ログ出力をするか
#define MEAS_DIFFS 0
// ヒストグラムの保持数
#define NUM_DIFFS 10

// FIXME 本当はIPipelineから直接継承したいんだけどなぜかいくつかの関数呼び出しで
// デストラクタが呼び出されてしまうのでAbstractBufferedPipelineから継承しておく
class PtsCalcPipeline : virtual public AbstractBufferedPipeline {
private:
	// カメラ側のstreamコンフィグのdwClockFrequencyの値/またはその補正値
	uint32_t _clock_frequency;
	uint32_t _prev_pts;
	uint32_t _next_seq;
	unsigned int _num_calc_frames;
	// ptsオーバーフロー時の加算用
	nsecs_t _pts_offset;
	// カメラ側で最初のフレームをキャプチャ開始した時に相当するシステム時刻[usec]
	nsecs_t _start_sys_time_us;
	void init_start_time(VideoFrame *frame,
		const nsecs_t &received_sys_time_us);
#if ENABLE_CLOCK_CORRECTION
	// ptsから計算した時刻とsystemTimeとの差分の累積値
	nsecs_t _sum_diff_us;
	// clock_frequency補正時の平均sum_diff_us[us]
	nsecs_t _init_ave_diff_us;
	// clock_frequency補正時のsystemTime[us]
	nsecs_t _init_sys_time_us;
	// clock_frequency補正間隔カウンタ
	uint32_t _correct_cnt;
	void clock_correction(VideoFrame *frame,
		const nsecs_t &received_sys_time_us,
	  const nsecs_t &cal_presentationtime_us);
#else
#define clock_correction(...)
#endif
#if CHECK_MONOTONIC
	nsecs_t _prev_calc_presentationtime_us;
	nsecs_t check_monotonic(const nsecs_t &calc_presentationtime_us);
#else
#define check_monotonic(calc) (calc)
#endif
#if MEAS_DIFFS
	nsecs_t _prev_meas_time;
	nsecs_t _diffs[NUM_DIFFS + 1];
	unsigned int _diff_cnt;
	void meas_diffs(VideoFrame *frame);
#else
#define meas_diffs(...)
#endif
	// pts計算の有効無効切り替えフラグ
	volatile bool _enabled;
	int calc_pts(VideoFrame *frame);
	int update_clock_frequency(const uint32_t &sequence);
	uint32_t get_packets_per_transfer();
	void internal_on_reset();
protected:
	// AbstractBufferedPipeline
	virtual void on_start();
	virtual void on_stop();
	virtual int handle_frame(VideoFrame *frame);
	// IPipeline
	virtual void on_reset();
	inline const uint32_t &clock_frequency() const { return _clock_frequency; };
	inline void clock_frequency(const uint32_t &clock_frequency) { _clock_frequency = clock_frequency; };
public:
	PtsCalcPipeline(const size_t &default_frame_size = DEFAULT_FRAME_SZ);
	virtual ~PtsCalcPipeline();

	inline const bool enabled() const { return _enabled; };
	virtual int release();
	virtual int start();
	virtual int stop();
	int setEnable(const bool &enable);
	virtual int queue_frame(VideoFrame *frame);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //AANDUSB_PIPELINE_PTS_CALC_H
