//
// Created by saki on 2018/11/19.
//

#ifndef MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H
#define MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H

#include <jni.h>

namespace serenegiant {
namespace usb {
namespace ncs {

class Movidius {
private:
	jobject movidius_obj;
	int fd;
protected:
public:
	Movidius(jobject obj);
	virtual ~Movidius();
	void release(JNIEnv *env);

	int connect(const int &fd);
	int disconnect();
};

}
}
}

//
#endif //MOVIDIUSTEST_SERENEGIANT_NCS_MOVIDIUS_H
