/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef AANDUSB_UVC_UTILS_H
#define AANDUSB_UVC_UTILS_H

#pragma interface

// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {
namespace uvc {

/**
 * IFrameCallback#onFrameメソッドのIDを取得する
 * @param env
 * @param frame_callback_obj
 * @param result
 * @return 0: success, others:error code
 */
int get_frame_callback_method(JNIEnv *env,
	jobject frame_callback_obj,
	Fields_iframecallback &result);

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //AANDUSB_UVC_UTILS_H
