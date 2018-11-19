/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_CALLBACK_H
#define PIPELINE_CALLBACK_H

// core
#include "core/internal.h"
// uvc
#include "uvc/aanduvc.h"
// uvc/pipeline
#include "pipeline_base_capture.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

/**
 * 受け取った映像をコールバックを介してJava側へ引き渡すためのパイプライン
 */
class CallbackPipeline : virtual public CaptureBasePipeline {
private:
	raw_frame_t callback_frame_type;
	jobject mFrameCallbackObj;
	Fields_iframecallback iframecallback_fields;
	const int sanitary_check;
protected:
	// CaptureBasePipeline
	virtual void do_capture(JNIEnv *env);
public:
	/**
	 * コンストラクタ
	 * @param _data_bytes
	 * @param _sanitary_check 0:何もしない, 1:ヘッダーチェックのみ(MJPEG), 2:サニタリーチェック(MJPEGのみ)
	 */
	CallbackPipeline(const size_t &_data_bytes = DEFAULT_FRAME_SZ, const int &_sanitary_check = 1);
	virtual ~CallbackPipeline();
	int set_frame_callback(JNIEnv *env, jobject frame_callback_obj,
		const raw_frame_t &_callback_frame_type);
};

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_CALLBACK_H
