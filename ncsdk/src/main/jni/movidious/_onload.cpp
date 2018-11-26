/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#define LOG_TAG "_onLoad"

#ifndef LOG_NDEBUG
#define	LOG_NDEBUG
#endif
#undef USE_LOGALL

#include "utilbase.h"
#include "_onload.h"
#include "common_utils.h"


extern int register_ncs_movidius_usbdatalink(JNIEnv *env);
extern int register_ncs_movidius_usbdatalink2(JNIEnv *env);
extern int register_ncs_mvnc_api(JNIEnv *env);

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("JNI_OnLoad");

    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    // register native methods
	int res = register_ncs_movidius_usbdatalink(env);
	env->ExceptionClear();
	CHECK(!res);
	res = register_ncs_movidius_usbdatalink2(env);
	env->ExceptionClear();
	CHECK(!res);
	res = register_ncs_mvnc_api(env);
	env->ExceptionClear();
	CHECK(!res);
	//
	setVM(vm);

    LOGD("JNI_OnLoad:finished:");
    return JNI_VERSION_1_6;
}
