/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UVC_SURFACE_PREVIEW_H
#define USBWEBCAMERAPROJ_UVC_SURFACE_PREVIEW_H

#include <queue>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include "assets.h"
// core
#include "core/internal.h"
// uvc
#include "aanduvc.h"
#include "uvc_frame_pool.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {

#define MAX_FRAME_PREVIEW 4
#define FRAME_POOL_SZ MAX_FRAME_PREVIEW + 2

class VideoFrame;

class UVCSurfacePreviewBase : virtual public UVCFramePool {
private:
	volatile bool mIsPreviewing;
	volatile bool mRequestResizePreview;
	uint32_t mPreviewWidth, mPreviewHeight;
	size_t mPreviewRawBytes;
	previewmode_t mPreviewMode;
	nsecs_t mMinFrameIntervalNs, mNextPreviewTimeNs;

	pthread_t preview_thread;				// プレビュースレッド
	mutable Mutex preview_mutex;			// ミューテックス
	Condition preview_sync;					// 同期オブジェクト
	mutable Mutex preview_queue_mutex;		// ミューテックス(プレビューキュー同期用)
	Condition preview_queue_sync;			// 同期オブジェクト(プレビューキュー同期用)
	std::queue<VideoFrame *> previewQueue;	// プレビューフレームキュー
	// プレビュースレッドの実行関数(pthreadから呼び出されるのでstaticじゃないとダメ)
	static void *preview_thread_func(void *vptr_args);
	bool mLowResolution;
	// キューからフレームを取り出してリサイクルするだけのループ
	void do_preview_recycle_only(JNIEnv *env);
	// mPreviewWindowがセットされてない時のダミーループ
	void do_preview_dummy(JNIEnv *env);
	// do_previewの下請け。来たフレームをyuvのままでSurfaceのformatに応じてrgb565またはrgbaとしてSurfaceへ書き込むループ
	void do_preview_yuyv_rgb(JNIEnv *env);
	// do_previewの下請け。来たフレームをyuvのままでrgbaとしてテクスチャにセット
	// GPUで変換してsurfaceへ書き込むループ
	void do_preview_gpu_rgba(JNIEnv *env);
	// do_previewの下請け。来たフレームをCPUでピクセルフォーマットをrgb565に変換してからプレビューsurfaceへ書き込むループ
	void do_preview_cpu_rgb565(JNIEnv *env);
	// do_previewの下請け。来たフレームをCPUでピクセルフォーマットをrgbaに変換してからプレビューsurfaceへ書き込むループ
	void do_preview_cpu_rgba(JNIEnv *env);
	// do_previewの下請け。来たフレームをCPUでMJPEG => yuyv変換してからrgbaとしてテクスチャにセット
	// GPUで変換してsurfaceへ書き込むループ
	void do_preview_mjpeg_gpu(JNIEnv *env);
	// do_previewの下請け。来たフレームをCPUでMJPEG => YCbCr planer変換してからGL_LUMINANCEとしてテクスチャにセット
	// GPUで変換してsurfaceへ書き込むループ
	void do_preview_mjpeg_gpu_ycbcr_planer(JNIEnv *env);
	// do_previewの下請け。来たフレームをCPUでMJPEG => yuyv => rgb/rgbx変換してからsurfaceへ書き込む
	void do_preview_mjpeg_cpu_rgb(JNIEnv *env);
	// do_previewの下請け。H264フレームをJava側へ送ってそっちでMediaCodecでデコード&Surfaceへ書き込む
	void do_preview_h264_mediacodec(JNIEnv *env);
	// do_previewの下請け。VP8フレームをJava側へ送ってそっちでMediaCodecでデコード&Surfaceへ書き込む
	void do_preview_vp8_mediacodec(JNIEnv *env);

	int copy_to_surface(VideoFrame *frame, ANativeWindow **window);
	int draw_preview_cpu_one(VideoFrame *frame, ANativeWindow **window, const raw_frame_t &frame_type, const int &pixelBytes);
	void internal_on_handle_frame(JNIEnv *env, VideoFrame *frame);
protected:
	Assets *mAssets;
	ANativeWindow *mPreviewWindow;

	UVCSurfacePreviewBase(Assets *assets);
	virtual ~UVCSurfacePreviewBase();
	virtual int release();

	const bool is_previewing() const;
	/** プレビュー開始要求 */
	virtual int start_preview();
	/** プレビュー処理終了要求(終了するまでブロックする) */
	virtual int stop_preview();
	/** プレビュー用Surfaceのサイズ変更要求 */
	virtual int request_resize(const previewmode_t &mode,
		const uint32_t &width, const uint32_t &height, const size_t &frame_raw_bytes);
	/** プレビュー処理ループ */
	virtual void do_preview(JNIEnv *env);
	/** プレビュー開始時のコールバック関数 */
	virtual void on_start_preview(JNIEnv *env) = 0;
	/** プレビュー停止時のコールバック関数 */
	virtual void on_stop_preview(JNIEnv *env) = 0;
	/** Surfaceのサイズ変更処理時のコールバック関数 */
	virtual void on_resize_preview(JNIEnv *env) = 0;
	/** Surfaceへの描画処理後の追加処理関数, falseを返すとrecycle_frameする */
	virtual bool on_handle_frame(JNIEnv *env, VideoFrame *frame) = 0;
	/** プレビュー用Surfaceの設定を行う */
	virtual int setup_preview_display() = 0;
	/** プレビューSurfaceを黒で塗りつぶす。EGLを使う場合には使っちゃダメ */
	void clearPreviewDisplay();
//================================================================================
	// プレビューフレームキューに追加する
	int add_preview_frame(VideoFrame *frame);
	// プレビューフレームキューからフレームを取得、キューが空の時はブロックする
	VideoFrame *wait_preview_frame();
	// プレビューフレームキューをクリアする
	void clear_preview_frame();
//================================================================================
public:
	/** プレビュー表示用のSurfaceをセット */
	int set_preview_display(ANativeWindow *preview_window, const bool &low_reso);
	int set_max_fps(const int &maxFps);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UVC_SURFACE_PREVIEW_H
