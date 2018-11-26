//
// Created by saki on 2018/11/21.
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
#include "usb_data_link2.h"

namespace serenegiant {
namespace usb {
namespace ncs {

UsbDataLink2::UsbDataLink2(jobject obj)
:	movidius_obj(obj),
	device(NULL),
	active(false)
{
	ENTER();
	
	EXIT();
}

UsbDataLink2::~UsbDataLink2() {
	ENTER();
	
	disconnect();
	context.release();	// XXX これは無くてもcontextが破棄される時にデストラクタから呼び出される

	EXIT();
}

void UsbDataLink2::release(JNIEnv *env) {
	ENTER();

	if (movidius_obj) {
		env->DeleteGlobalRef(movidius_obj);
		movidius_obj = NULL;
	}

	EXIT();
}

int UsbDataLink2::connect(const int &fd) {
	ENTER();

	usb_error_t result = USB_ERROR_BUSY;
	device = new Device(&context, &descriptor, fd);
	if (device && device->is_active()) {
		device->claim_interface(0);
		active = true;
		result = USB_SUCCESS;
	} else {
		LOGE("could not find UsbDataLink2:err=%d", result);
		SAFE_DELETE(device);
	}

	RETURN(result, int);
}

int UsbDataLink2::disconnect() {
	ENTER();

	active = false;
	if (device) {
		device->release_interface(0, false);
	}
	SAFE_DELETE(device);

	RETURN(USB_SUCCESS, int);
}

const bool UsbDataLink2::is_active() const {
	return active && device && device->is_active();
};

int UsbDataLink2::reset() {
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

int UsbDataLink2::get_status(myriadStatus_t &myriad_state) {
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

int UsbDataLink2::set_data(const char *name,
	const void *data, const uint32_t &length,
	const bool &host_ready) {

	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_HOST_SET_DATA;
	header.hostready = (uint8_t)(host_ready ? 1 : 0);
	strcpy(header.name, name);
	header.dataLength = length;
	int err = write(&header, sizeof(usbHeader_t));
	if (UNLIKELY(err)) {
		LOGE("write failed, err=%d", err);
		RETURN(err, int);
	}
	uint32_t operation_permit = 0xFFFF;
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

int UsbDataLink2::get_data(const char *name,
	void *data, const uint32_t &length, const uint32_t &offset,
	const bool &host_ready) {

	ENTER();
	
	usbHeader_t header;
	memset(&header, 0, sizeof(usbHeader_t));
	header.cmd = USB_LINK_HOST_GET_DATA;
	header.hostready = (uint8_t)(host_ready ? 1 : 0);
	strcpy(header.name, name);
	header.dataLength = length;
	header.offset = offset;
	int err = write(&header, sizeof(usbHeader_t));
	if (UNLIKELY(err)) {
		LOGE("write failed, err=%d", err);
		RETURN(err, int);
	}

	uint32_t operation_permit = 0xFFFF;
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

//================================================================================
#define USB_ENDPOINT_IN 	0x81
#define USB_ENDPOINT_OUT 	0x01
#define USB_TIMEOUT 		10000
#define USB_MAX_PACKET_SIZE	1024 * 1024 * 10

/*private*/
int UsbDataLink2::write(const void *data, const size_t &size) {
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
int UsbDataLink2::read(void *data, const size_t &size) {
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
