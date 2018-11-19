/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_SAMPLER_H_
#define UAC_SAMPLER_H_

#include <jni.h>
#include <vector>
#include <queue>
#include <pthread.h>
// uac
#include "uac/uac_frame_pool.h"

namespace serenegiant {
namespace usb {
namespace uac {

class UACDevice;
class UACDescriptor;
class UACStream;
class AudioFrame;

/**
*
*/
class UACSampler : virtual public UACFramePool {
private:
	// Java側インスタンスへのコールバック用
	jobject uacclient_obj;
	jmethodID jmethodID_onData;
	volatile bool mIsRunning;	// 音声取得中(ストリーム中)
	// UAC機器アクセス用
	UACDevice *device;
	UACDescriptor *descriptor;
	std::vector<UACStream *> streams;
	UACStream *stream;
	uac_stream_ctrl currentControl;
	//
	bool prepared;
	uac_raw_format_t req_format = UAC_RAW_FORMAT_I_PCM;
	int req_max_channels;		// 最大チャネル数
	int req_max_resolution;		// 解像度(ビット数)
	int req_min_sampling_freq;	// 最小サンプリング周波数
	int req_max_sampling_freq;	// 最大サンプリング周波数
	int req_sampling_freq;		// 出力サンプリング周波数(今は44100Hzに固定)
	// フレーム毎のコールバック関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static void uac_frame_callback(AudioFrame *frame, void *frame_callback_args);
	// コールバック用フレームキュー関係
	std::queue<AudioFrame *> mCallbackFrames;	// フレームキュー
	mutable Mutex callback_queue_mutex;
	Condition callback_queue_sync;
	void add_callback_frame(AudioFrame *frame);
	AudioFrame *wait_callback_frame();
	void clear_callback_frame();
	// コールバックスレッド
	pthread_t callback_thread;			// コールバックスレッド
	mutable Mutex  callback_mutex;		// ミューテックス
	Condition callback_sync;			// 同期オブジェクト
	static void *callback_thread_func(void *vptr_args);
	void do_callback(JNIEnv *env);
	void do_callback_loop(JNIEnv *env);
	void do_callback_loop_resample_up(JNIEnv *env);
	void do_callback_loop_resample_up_1ch(JNIEnv *env);
	void do_callback_loop_resample_up_2ch(JNIEnv *env);
	void do_callback_loop_resample_down(JNIEnv *env);
	void do_callback_loop_resample_down_1ch(JNIEnv *env);
	void do_callback_loop_resample_down_2ch(JNIEnv *env);
protected:
	void after_stop_callback();
	UACDevice *get_device() { return device; };
	int on_frame(JNIEnv *env, AudioFrame *frame);
public:
	UACSampler(UACDevice *_device, jobject _uacclient_obj);
	virtual ~UACSampler();
	int start();
	int stop();
	inline const bool is_running() const {return mIsRunning; }
	int prepare_stream(JNIEnv *env,
		const uac_raw_format_t &format,
		const int &max_channels,		// 最大チャネル数
		const int &max_resolution,		// 解像度(ビット数)
		const int &min_sampleing_freq,	// 最小サンプリング周波数
		const int &max_sampleing_freq);	// 最大サンプリング周波数
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_SAMPLER_H_ */
