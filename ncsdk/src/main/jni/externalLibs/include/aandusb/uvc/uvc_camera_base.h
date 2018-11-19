/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UVC_CAMERA_BASE_H
#define USBWEBCAMERAPROJ_UVC_CAMERA_BASE_H

#pragma interface

// uvc
#include "aanduvc.h"
#include "uvc_frame_pool.h"

namespace serenegiant {
namespace usb {

class Device;

namespace uvc {

using namespace android;

typedef struct stream_config {
public:
    raw_format_t rawFormatType;
    raw_frame_t rawFrameType;
    int formatIndex;
    int frameIndex;
	uint32_t width, height;
    uint32_t frameInterval;
    uint32_t clock_frequency;
    uint32_t packets_per_transfer;

    void clear() {
        rawFormatType = RAW_FORMAT_UNCOMPRESSED;
        rawFrameType = RAW_FRAME_UNCOMPRESSED_YUYV;
        formatIndex = 0;
        frameIndex = 0;
        width = height = 0;
        frameInterval = 0;
		clock_frequency = 0;
		packets_per_transfer = 0;
    }
} stream_config_t;

class UVCDescriptor;
class UVCStream;
class VideoFrame;

class UVCCameraBase : virtual public UVCFramePool {
private:
	const bool enable_capture_method;
	UVCDescriptor *descriptor;
	Device *device;
	volatile bool is_aux_stream;		// 外部ストリームからのストリーミングかどうか
	int cull_steps;
	volatile bool mIsStreaming;
	volatile bool mRequestResize;		// リサイズ要求

	pthread_t stream_thread;			// カメラストリームスレッド
	// ストリームスレッドの実行関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static void *stream_thread_func(void *vptr_args);
	// フレーム毎のコールバック関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static int uvc_frame_callback(VideoFrame *frame, void *vptr_args);
	int update_current_config();
	int find_stream(UVCStream *stream);
protected:
	// UVCカメラアクセス用
	std::vector<UVCStream *> streams;
	UVCStream *current_stream;
//----------------------------------------------------------------------

	mutable Mutex stream_mutex;			// ミューテックス
	Condition stream_sync;				// 同期オブジェクト

// 要求値
	uint32_t requestWidth, requestHeight;
	float requestFpsMin, requestFpsMax;
	float requestBandWidthFactor;
	raw_frame_t requestRawFrameType;
	previewmode_t requestPreviewMode;
// メインストリームの現在値
    stream_config_t currentStreamConfig;
// 外部ストリームの現在値
    stream_config_t currentAuxStreamConfig;
// ストリーム中の値
    uint32_t streamWidth, streamHeight;		// ストリーム中の映像サイズ
	size_t mRawFrameBytes;				// ストリーム中の生フレームデータのサイズ(yuv)
	size_t mAuxRawFrameBytes;			// 外部ストリームの生フレームデータのサイズ(yuv), 未使用

//----------------------------------------------------------------------
	UVCCameraBase(Device *device,
		const uint32_t &max_frame_num = DEFAULT_MAX_FRAME_NUM,
		const uint32_t &init_frame_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &_default_frame_sz = DEFAULT_FRAME_SZ,
		const bool &create_if_empty = false,
		const bool &block_if_empty = false,
		const uint32_t &num_transfer_bufs = NUM_TRANSFER_BUFS,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE,
		const bool &enable_capture_method = false);

	virtual ~UVCCameraBase();
	virtual int release();
	/** カメラ映像を受け取った時の処理
	 * UVCStreamのフレームコールバックスレッド内で呼ばれるので
	 * 可能な限り速く処理する事。フレームサイズチェックは済 */
	virtual void on_frame_callback(VideoFrame *frame) = 0;
	const bool is_enable_capture_method () const { return enable_capture_method; };
	const bool is_streaming() const { return mIsStreaming; };
	const bool need_resize() const { return mRequestResize; };
	void set_request_resize(const bool &request) { mRequestResize = request; };
	/** ネゴシエーション実行 */
	int find_stream(const bool &can_change_current = true);
	int prepare_stream();
	/** ストリーミング開始 */
	virtual int internal_start_stream();
	virtual int internal_stop_stream();
	virtual void do_stream(JNIEnv *env) = 0;
	virtual void on_start_streaming() = 0;
	virtual void on_stop_streaming() = 0;
	/** カメラストリーム停止時の処理(カメラストリームスレッドへの停止要求時に呼び出される) */
	virtual void after_stop_stream() = 0;
	virtual int restart_stream();
	virtual still_capture_method_t request_take_picture(
		const uint32_t &width, const uint32_t &height,
		still_capture_callback_t callback, void *usr_args);
public:
	int set_frame_size(const previewmode_t &mode,        // FIXME これはuvc_raw_frame_tに変えたい
		const uint32_t &width = DEFAULT_PREVIEW_WIDTH,
		const uint32_t &height = DEFAULT_PREVIEW_HEIGHT,
		const float &min_fps = DEFAULT_PREVIEW_FPS_MIN,
		const float &max_fps = DEFAULT_PREVIEW_FPS_MAX,
		const float &bandwidth_factor = 0.0f);
	// JSON形式でカメラの現在のストリーム設定を返す
	// 外部ストリームが有効ならば外部ストリーム設定を取得
	// 外部ストリーム設定が無効ならばメインストリームを取得
	char *get_current_stream();
	// JSON形式で現在の外部ストリーム状態を返す, 外部ストリームが無い時は空
	char *getCurrentAuxStream();
	inline const uint32_t &clock_frequency() const { return currentStreamConfig.clock_frequency; };
	inline const uint32_t &packets_per_transfer() const { return currentStreamConfig.packets_per_transfer; };
	const still_capture_method_t get_still_capture_method();
	virtual int start_stream(const int &_cull_steps = 0);
	virtual int stop_stream();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UVC_CAMERA_BASE_H
