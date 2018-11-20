//
// Created by saki on 2018/11/19.
//

#ifndef MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H
#define MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H

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

class Movidius {
private:
	jobject movidius_obj;
	Context context;
	Descriptor descriptor;
	Device *device;
	mutable Mutex lock;
	volatile bool active;
	
	int write(const void *data, const size_t &size);
	int read(void *data, const size_t &size);
protected:
public:
	Movidius(jobject obj);
	virtual ~Movidius();
	void release(JNIEnv *env);

	int connect(const int &fd);
	int disconnect();

	inline const bool is_active() const { return active && device && device->is_active(); };

	int reset();
	int get_status(myriadStatus_t &myriadState);
	int set_data(const char *name,
		const void *data, const unsigned int &length,
		const uint8_t &host_ready);
	int get_data(const char *name,
		void *data, const unsigned int &length, const unsigned int &offset,
		const uint8_t &host_ready);
	int reset_all();
};

}
}
}

//
#endif //MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H
