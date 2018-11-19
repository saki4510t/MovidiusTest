/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_DEVICE_H_
#define UPC_DEVICE_H_

// core
#include "core/device.h"
// upc
#include "androupc.h"

/**
*
*/
class UPCDevice : public Device {
protected:
	void dump_vs_streaming_control(const streaming_control_t &desc);
public:
	UPCDevice(Context *_context, Descriptor *_descriptor, const char *dev_name, int fd);
	virtual ~UPCDevice();
	int query_ctrl(streaming_control_t &ctrl, const uint8_t &interface_number, const bool &probe, const req_code_t &req);
};


#endif /* UPC_DEVICE_H_ */
