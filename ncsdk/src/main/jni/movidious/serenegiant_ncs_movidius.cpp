//
// Created by saki on 2018/11/19.
//

#if 0	// set 0 is you need debug log, otherwise set 1
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
#include "serenegiant_ncs_movidius.h"

namespace serenegiant {
namespace usb {
namespace ncs {

Movidius::Movidius(jobject obj)
:	movidius_obj(obj),
	device(NULL)
{
	ENTER();
	
	EXIT();
}

Movidius::~Movidius() {
	ENTER();
	
	disconnect();
	context.release();	// XXX これは無くてもcontextが破棄される時にデストラクタから呼び出される

	EXIT();
}

void Movidius::release(JNIEnv *env) {
	ENTER();

	if (movidius_obj) {
		env->DeleteGlobalRef(movidius_obj);
		movidius_obj = NULL;
	}

	EXIT();
}

int Movidius::connect(const int &fd) {
	ENTER();

	usb_error_t result = USB_ERROR_BUSY;
	device = new Device(&context, &descriptor, fd);
	if (device && device->is_active()) {
		// FIXME 未実装
		result = USB_SUCCESS;
	} else {
		LOGE("could not find Movidius:err=%d", result);
		SAFE_DELETE(device);
	}

	RETURN(result, int);
}

int Movidius::disconnect() {
	ENTER();

	// FIXME 未実装

	SAFE_DELETE(device);

	RETURN(USB_SUCCESS, int);
}

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant
//**********************************************************************
//
//**********************************************************************

using namespace serenegiant::usb::ncs;

static ID_TYPE nativeCreate(JNIEnv *env, jobject thiz) {

	ENTER();

	jobject obj = env->NewGlobalRef(thiz);
	//
	Movidius *ncs
		= new Movidius(obj);
	setField_long(env, thiz, "mNativePtr", reinterpret_cast<ID_TYPE>(ncs));
	LOGD("ncs=%p", ncs);

	RETURN(reinterpret_cast<ID_TYPE>(ncs), ID_TYPE);
}

// native側のカメラオブジェクトを破棄
static void nativeDestroy(JNIEnv *env, jobject thiz,
	ID_TYPE id_ncs) {

	ENTER();

	setField_long(env, thiz, "mNativePtr", 0);
	Movidius *ncs = reinterpret_cast<Movidius *>(id_ncs);
	if (LIKELY(ncs)) {
		ncs->release(env);
		SAFE_DELETE(ncs);
	}

	EXIT();
}

// カメラへ接続
static jint nativeConnect(JNIEnv *env, jobject thiz,
	ID_TYPE id_ncs, jint fd) {

	ENTER();

	int result = JNI_ERR;
	Movidius *ncs = reinterpret_cast<Movidius *>(id_ncs);
	if (LIKELY(ncs && (fd > 0))) {
		fd = dup(fd);	// Java側からcloseされてしまわないようにする
		result = ncs->connect(fd);
	}

	RETURN(result, jint);
}

// カメラとの接続を解除
static jint nativeDisconnect(JNIEnv *env, jobject thiz,
	ID_TYPE id_pipeline) {

	ENTER();

	int result = JNI_ERR;
	Movidius *ncs = reinterpret_cast<Movidius *>(id_pipeline);
	if (LIKELY(ncs)) {
		result = ncs->disconnect();
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
};

int register_ncs_movidius(JNIEnv *env) {
	ENTER();

	// ネイティブメソッドを登録
	if (registerNativeMethods(env,
		"com/serenegiant/ncsdk/Movidius",
		methods, NUM_ARRAY_ELEMENTS(methods)) < 0) {
		RETURN(-1, int);
	}

	RETURN(0, int);
}

