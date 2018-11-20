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
	device(NULL),
	active(false)
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
		device->claim_interface(0);
		active = true;
		result = USB_SUCCESS;
	} else {
		LOGE("could not find Movidius:err=%d", result);
		SAFE_DELETE(device);
	}

	RETURN(result, int);
}

int Movidius::disconnect() {
	ENTER();

	active = false;
	if (device) {
		device->release_interface(0, false);
	}
	SAFE_DELETE(device);

	RETURN(USB_SUCCESS, int);
}

int Movidius::reset() {
	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_RESET_REQUEST;
	int err = write(&header, sizeof(usbHeader_t));
	if (UNLIKELY(err)) {
		LOGE("write failed,err=%d", err);
	}

	RETURN(err, int);
}

int Movidius::get_status(myriadStatus_t &myriad_state) {
	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_GET_MYRIAD_STATUS;
	int err = write(&header, sizeof(header));
	if (LIKELY(!err)) {
		err = read(&myriad_state, sizeof(myriadStatus_t));
		if (UNLIKELY(err)) {
			LOGE("read failed,err=%d", err);
		}
	} else {
		LOGE("write failed,err=%d", err);
	}

	RETURN(err, int);
}

int Movidius::set_data(const char *name,
	const void *data, const unsigned int &length,
	const uint8_t &host_ready) {

	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_HOST_SET_DATA;
	header.hostready = host_ready;
	strcpy(header.name, name);
	header.dataLength = length;
	int err = write(&header, sizeof(usbHeader_t));
	if (UNLIKELY(err)) {
		LOGE("write failed, err=%d", err);
		RETURN(err, int);
	}
	unsigned int operation_permit = 0xFFFF;
	err = read(&operation_permit, sizeof(operation_permit));
	if (UNLIKELY(err)) {
		LOGE("read failed, err=%d", err);
		RETURN(err, int);
	}

	if (operation_permit == 0xABCD) {
		err = write(data, length);
	} else {
		err = USB_ERROR_BUSY;
	}

	RETURN(err, int);
}

int Movidius::get_data(const char *name,
	void *data, const unsigned int &length, const unsigned int &offset,
	const uint8_t &host_ready) {

	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_HOST_GET_DATA;
	header.hostready = host_ready;
	strcpy(header.name, name);
	header.dataLength = length;
	header.offset = offset;
	int err = write(&header, sizeof(usbHeader_t));
	if (UNLIKELY(err)) {
		LOGE("write failed, err=%d", err);
		RETURN(err, int);
	}

	unsigned int operation_permit = 0xFFFF;
	err = read(&operation_permit, sizeof(operation_permit));
	if (LIKELY(!err)) {
		if (operation_permit == 0xABCD) {
			err = read(data, length);
		} else {
			err = USB_ERROR_BUSY;
		}
	} else {
		LOGE("read failed, err=%d", err);
		RETURN(err, int);
	}

	RETURN(err, int);
}

int Movidius::reset_all() {
	ENTER();
	
	// FIXME 未実装

	RETURN(-1, int);
}

//================================================================================
#define USB_ENDPOINT_IN 	0x81
#define USB_ENDPOINT_OUT 	0x01
#define USB_TIMEOUT 		10000
#define USB_MAX_PACKET_SIZE	1024 * 1024 * 10

/*private*/
int Movidius::write(const void *data, const size_t &size) {
	ENTER();
	
	int result = -1;
	if (is_active()) {
		size_t offset = 0;
		uint8_t *buffer = (uint8_t *)data;
		int wb;
		for (; is_active() && (offset < size);) {
			wb = (int)(size - offset);
			if (wb > USB_MAX_PACKET_SIZE) {
				wb = USB_MAX_PACKET_SIZE;
			}
			const int write_bytes = device->bulk_transfer_sync(
				USB_ENDPOINT_OUT, buffer, wb, USB_TIMEOUT);
			if (write_bytes <= 0) {
				break;
			}
			offset += write_bytes;
			buffer = buffer + write_bytes;
		}
		if (offset == size) {
			result = USB_SUCCESS;
		} else {
			result = USB_ERROR_INTERRUPTED;
		}
	}

	RETURN(result, int);
}

/*private*/
int Movidius::read(void *data, const size_t &size) {
	ENTER();
	
	int result = -1;
	if (is_active()) {
		size_t offset = 0;
		uint8_t *buffer = (uint8_t *)data;
		int rb;
		for (; is_active() && (offset < size);) {
			rb = (int)(size - offset);
			if (rb > USB_MAX_PACKET_SIZE) {
				rb = USB_MAX_PACKET_SIZE;
			}
			const int read_bytes = device->bulk_transfer_sync(
				USB_ENDPOINT_IN, buffer, rb, USB_TIMEOUT);
			if (read_bytes <= 0) {
				break;
			}
			offset += read_bytes;
			buffer = buffer + read_bytes;
		}
		if (offset == size) {
			result = USB_SUCCESS;
		} else {
			result = USB_ERROR_INTERRUPTED;
		}
	}

	RETURN(result, int);
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

