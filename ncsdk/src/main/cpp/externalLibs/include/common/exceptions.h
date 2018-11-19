/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_EXCEPTIONS_H_H
#define USBWEBCAMERAPROJ_EXCEPTIONS_H_H

#include <jni.h>

int throwException(JNIEnv *env, const char *exceptionFQN, const char *message);
int throwException(JNIEnv *env, const char *exceptionFQN);
//--------------------------------------------------------------------------------
int throwIOException(JNIEnv *env, const char *message);
int throwIOException(JNIEnv *env);
//--------------------------------------------------------------------------------
int throwIllegalStateException(JNIEnv *env, const char *message);
int throwIllegalStateException(JNIEnv *env);
//--------------------------------------------------------------------------------
// java.io.InterruptedIOException extends java.io.IOException
int throwInterruptedIOException(JNIEnv *env, const char *message);
int throwInterruptedIOException(JNIEnv *env);
//--------------------------------------------------------------------------------
int throwIllegalArgumentException(JNIEnv *env, const char *message);
int throwIllegalArgumentException(JNIEnv *env);

#endif //USBWEBCAMERAPROJ_EXCEPTIONS_H_H
