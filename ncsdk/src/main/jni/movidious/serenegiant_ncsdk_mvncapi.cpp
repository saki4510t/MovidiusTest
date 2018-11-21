//
// Created by saki on 2018/11/21.
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
// common
#include "utilbase.h"
#include "common_utils.h"
// ncsdk
#include "usb_data_link.h"
#include "mvnc_api.h"
#include "serenegiant_ncsdk_mvncapi.h"

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

static ID_TYPE nativeCreate(JNIEnv *env, jobject thiz) {

	ENTER();

	MvNcApi *api = new MvNcApi();
	setField_long(env, thiz, "mNativePtr", reinterpret_cast<ID_TYPE>(api));
	LOGD("api=%p", api);

	RETURN(reinterpret_cast<ID_TYPE>(api), ID_TYPE);
}

// native側のオブジェクトを破棄
static void nativeDestroy(JNIEnv *env, jobject thiz,
	ID_TYPE id_api) {

	ENTER();

	setField_long(env, thiz, "mNativePtr", 0);
	MvNcApi *api = reinterpret_cast<MvNcApi *>(id_api);
	if (LIKELY(api)) {
		SAFE_DELETE(api);
	}

	EXIT();
}

// UsbDataLinkオブジェクトをMvNcAPIオブジェクトに追加
static int nativeAddDataLink(JNIEnv *env, jobject thiz,
	ID_TYPE id_api, ID_TYPE id_datalink) {

	ENTER();

	int result = -1;
	MvNcApi *api = reinterpret_cast<MvNcApi *>(id_api);
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(api && datalink)) {
		result = api->add(datalink);
	}

	RETURN(result, int);
}

// UsbDataLinkオブジェクトをMvNcAPIオブジェクトから除去
static int nativeRemoveDataLink(JNIEnv *env, jobject thiz,
	ID_TYPE id_api, ID_TYPE id_datalink) {

	ENTER();

	int result = -1;
	MvNcApi *api = reinterpret_cast<MvNcApi *>(id_api);
	UsbDataLink *datalink = reinterpret_cast<UsbDataLink *>(id_datalink);
	if (LIKELY(api && datalink)) {
		result = api->remove(datalink);
	}

	RETURN(result, int);
}

//**********************************************************************
//
//**********************************************************************

static JNINativeMethod methods[] = {
	{ "nativeCreate",			"()J", (void *) nativeCreate },
	{ "nativeDestroy",			"(J)V", (void *) nativeDestroy },
	{ "nativeAddDataLink",		"(JJ)I", (void *) nativeAddDataLink },
	{ "nativeRemoveDataLink",	"(JJ)I", (void *) nativeRemoveDataLink },
};

int register_ncs_mvnc_api(JNIEnv *env) {
	ENTER();

	// ネイティブメソッドを登録
	if (registerNativeMethods(env,
		"com/serenegiant/ncsdk/MvNcAPI",
		methods, NUM_ARRAY_ELEMENTS(methods)) < 0) {
		RETURN(-1, int);
	}

	RETURN(0, int);
}

