//
// Created by saki on 2018/11/21.
//

#ifndef MOVIDIUSTEST_USB_DATA_LINK2_H
#define MOVIDIUSTEST_USB_DATA_LINK2_H

#include <jni.h>

// core
#include "core/context.h"
#include "core/descriptor.h"
#include "core/device.h"
// ncsdk
#include "USBLinkDefines.h"

namespace serenegiant {
namespace usb {
namespace ncs {

class UsbDataLink2 {
private:
	jobject movidius_obj;
	Context context;
	Descriptor descriptor;
	Device *device;
	mutable Mutex lock;
	volatile bool active;
	
protected:
public:
	UsbDataLink2(jobject obj);
	virtual ~UsbDataLink2();
	void release(JNIEnv *env);

	virtual int connect(const int &fd);
	virtual int disconnect();

	virtual const bool is_active() const;

	virtual int reset();
	virtual int get_status(myriadStatus_t &myriadState);
	virtual int set_data(const char *name,
		const void *data, const uint32_t &length,
		const bool &host_ready);
	virtual int get_data(const char *name,
		void *data, const uint32_t &length, const uint32_t &offset,
		const bool &host_ready);
	int write(const void *data, const size_t &size);
	int read(void *data, const size_t &size);
};

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant

#endif //MOVIDIUSTEST_USB_DATA_LINK2_H
