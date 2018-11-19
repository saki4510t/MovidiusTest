/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_CLIENT_H_
#define UAC_CLIENT_H_

#include <jni.h>
// core
#include "core/context.h"
// uac
#include "uac/uac_descriptor.h"

namespace serenegiant {
namespace usb {
namespace uac {

class UACDevice;
class AudioFrame;
class UACSampler;
class UACControl;

/**
*
*/
class UACClient {
private:
	jobject uacclient_obj;

	Context context;
	UACDevice *device;
	UACDescriptor descriptor;
	UACSampler *sampler;
	UACControl *ctrl;
protected:
public:
	UACClient(jobject obj);
	virtual ~UACClient();
	void release(JNIEnv *env);

	int connect(JNIEnv *env, const char *dev_name, int vid, int pid, int fd);
	int disconnect();
	// 指定したパスへディスクリプタ等を出力する
	int output_descriptor(const char *path);
	// JSON形式で現在のストリーム状態を返す
	char *get_current_stream();

	int start();
	int stop();
	const bool is_running() const;
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_CLIENT_H_ */
