/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef PIPELINE_HELPER_H
#define PIPELINE_HELPER_H

#include <jni.h>
// uvc/pipeline
#include "pipeline_base.h"

namespace serenegiant {
namespace usb {
namespace uvc {
namespace pipeline {

IPipeline *get_pipeline(JNIEnv *env, jobject pipeline_obj);

}	// end of namespace pipeline
}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //PIPELINE_HELPER_H
