/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef LOCALDEFINES_H_
#define LOCALDEFINES_H_

#include <jni.h>

#ifndef LOG_TAG
#define LOG_TAG "movidius"
#endif

#define LIBUVC_HAS_JPEG

// write back array that got by getXXXArrayElements into original Java object and release its array
#define	ARRAYELEMENTS_COPYBACK_AND_RELEASE 0
// write back array that got by getXXXArrayElements into original Java object but do not release its array
#define	ARRAYELEMENTS_COPYBACK_ONLY JNI_COMMIT
// never write back array that got by getXXXArrayElements but release its array
#define ARRAYELEMENTS_ABORT_AND_RELEASE JNI_ABORT

#define THREAD_PRIORITY_DEFAULT			0
#define THREAD_PRIORITY_LOWEST			19
#define THREAD_PRIORITY_BACKGROUND		10
#define THREAD_PRIORITY_FOREGROUND		-2
#define THREAD_PRIORITY_DISPLAY			-4
#define THREAD_PRIORITY_URGENT_DISPLAY	-8
#define THREAD_PRIORITY_AUDIO			-16
#define THREAD_PRIORITY_URGENT_AUDIO	-19

//#define USE_LOGALL	// If you don't need to all LOG, comment out this line and select follows
//#define USE_LOGV
//#define USE_LOGD
#define USE_LOGI
#define USE_LOGW
#define USE_LOGE
#define USE_LOGF

#ifdef NDEBUG
#undef USE_LOGALL
#endif

#ifdef LOG_NDEBUG
#undef USE_LOGALL
#endif

// Absolute class name of Java object
// if you change the package name of AndroBulletGlue library, you must fix these
#define		JTYPE_SYSTEM				"Ljava/lang/System;"
#define		JTYPE_STRING				"Ljava/lang/String;"
#define		JTYPE_CONTEXT				"Landroid/content/Context;"
#define		JTYPE_ACTIVITY				"Landroid/app/Activity;"
#define		JTYPE_WINDOWMANAGER			"Landroid.view.WindowManager;"
#define		JTYPE_DISPLAY				"Landroid/view/Display;"
#define		JTYPE_DISPLAYMETRICS		"Landroid/util/DisplayMetrics;"
//
//
typedef		jlong						ID_TYPE;

#if defined(__LP64__)
#define FMT_SIZE_T "lu"
#else
#define FMT_SIZE_T "u"
#endif

#endif /* LOCALDEFINES_H_ */
