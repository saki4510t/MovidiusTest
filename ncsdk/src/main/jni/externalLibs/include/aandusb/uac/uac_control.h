/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_CONTROL_H_
#define UAC_CONTROL_H_

// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {

class Device;

namespace uac {

class AudioControlInterface;

/**
*
*/
class UACControl {
private:
	Device *device;
	AudioControlInterface *interface;
	int bInterfaceNumber;
public:
	UACControl(AudioControlInterface *_interface);
	virtual ~UACControl();
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_CONTROL_H_ */
