/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_DEVICE_H_
#define UAC_DEVICE_H_

// core
#include "core/device.h"
// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {
namespace uac {

/**
*
*/
class UACDevice : public Device {
protected:
public:
	UACDevice(Context *_context, Descriptor *_descriptor, const char *dev_name, int fd);
	virtual ~UACDevice();
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_DEVICE_H_ */
