//
// Created by saki on 2018/11/19.
//

#if 1	// set 0 is you need debug log, otherwise set 1
	#ifndef LOG_NDEBUG
		#define	LOG_NDEBUG
	#endif
	#undef USE_LOGALL
#else
	#define USE_LOGALL
	#undef LOG_NDEBUG
	#undef NDEBUG
#endif

#include <jni.h>

#include "utilbase.h"
#include "common_utils.h"

// core
#include "core/context.h"
#include "core/device.h"
#include "core/descriptor.h"
#include "core/interface.h"
#include "core/transfer.h"
// ncs
#include "serenegiant_ncsdk_usbdatalink.h"
#include "usb_data_link.h"

namespace serenegiant {
namespace usb {
namespace ncs {


}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant
//**********************************************************************
//
//**********************************************************************

using namespace serenegiant::usb::ncs;

// native側オブジェクトを生成
static ID_TYPE nativeCreate(JNIEnv *env, jobject thiz) {

	ENTER();

	jobject obj = env->NewGlobalRef(thiz);
	//
	UsbDataLink *datalink = new UsbDataLink(obj);
	setField_long(env, thiz, "mNativePtr", reinterpret_cast<ID_TYPE>(datalink));
	LOGD("datalink=%p", datalink);

	RETURN(reinterpret_cast<ID_TYPE>(datalink), ID_TYPE);
}

// native側のオブジェクトを破棄
static void nativeDestroy(JNIEnv *env, jobject thiz,
	ID_TYPE id_datalink) {

	ENTER();

	setField_long(env, thiz, "mNativePtr", 0);
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(datalink)) {
		datalink->release(env);
		SAFE_DELETE(datalink);
	}

	EXIT();
}

// データリンクを接続
static jint nativeConnect(JNIEnv *env, jobject thiz,
	ID_TYPE id_datalink, jint fd) {

	ENTER();

	int result = JNI_ERR;
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(datalink && (fd > 0))) {
		fd = dup(fd);	// Java側からcloseされてしまわないようにする
		result = datalink->connect(fd);
	}

	RETURN(result, jint);
}

// データリンクを切断
static jint nativeDisconnect(JNIEnv *env, jobject thiz,
	ID_TYPE id_datalink) {

	ENTER();

	int result = JNI_ERR;
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(datalink)) {
		result = datalink->disconnect();
	}

	RETURN(result, jint);
}

// データリンクをリセット
static jint nativeReset(JNIEnv *env, jobject thiz,
	ID_TYPE id_datalink) {

	ENTER();

	int result = JNI_ERR;
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(datalink)) {
		result = datalink->reset();
	}

	RETURN(result, jint);
}

//**********************************************************************
//
//**********************************************************************

static JNINativeMethod methods[] = {
	{ "nativeCreate",		"()J", (void *) nativeCreate },
	{ "nativeDestroy",		"(J)V", (void *) nativeDestroy },
	{ "nativeConnect",		"(JI)I", (void *) nativeConnect },
	{ "nativeDisConnect",	"(J)I", (void *) nativeDisconnect },
	{ "nativeReset",		"(J)I", (void *) nativeReset },
};

int register_ncs_movidius(JNIEnv *env) {
	ENTER();

	// ネイティブメソッドを登録
	if (registerNativeMethods(env,
		"com/serenegiant/ncsdk/UsbDataLink",
		methods, NUM_ARRAY_ELEMENTS(methods)) < 0) {
		RETURN(-1, int);
	}

	RETURN(0, int);
}

