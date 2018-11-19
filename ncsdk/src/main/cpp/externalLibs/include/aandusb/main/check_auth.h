/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef AANDUSB_CHECK_AUTH_H
#define AANDUSB_CHECK_AUTH_H

#include <jni.h>
#include "time_utc.h"

extern "C" int check_auth(
	JavaVM *vm, JNIEnv *env,
	jobject context_obj, const bool &force,
	int &l, TIME_T &e);

#endif //AANDUSB_CHECK_AUTH_H
