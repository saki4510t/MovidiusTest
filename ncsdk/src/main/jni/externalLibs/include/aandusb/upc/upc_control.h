/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_CONTROL_H_
#define UPC_CONTROL_H_

// upc
#include "androupc.h"

class Device;
class VideoControlInterface;

/**
*
*/
class UPCControl {
private:
	Device *device;
	VideoControlInterface *interface;
	int bInterfaceNumber;
protected:
public:
	UPCControl(VideoControlInterface *interface);
	virtual ~UPCControl();
};

#endif /* UPC_CONTROL_H_ */
