/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_INTERFACE_H_
#define UPC_INTERFACE_H_

// core
#include "core/interface.h"
// upc
#include "androupc.h"

/**
*
*/
class VideoControlInterface : public Interface {
private:
protected:
public:
	VideoControlInterface(Device *device, const interface_descriptor_t *desc);
	virtual ~VideoControlInterface();
};

/**
*
*/
class VideoInputStreamInterface : public Interface {
private:
protected:
public:
	VideoInputStreamInterface(Device *device, const interface_descriptor_t *desc);
	virtual ~VideoInputStreamInterface();
	int findAltSetting(const size_t &dwMaxVideoFrameSize, const size_t &bytes_per_packet,
		Endpoint *&result, size_t &packets_per_transfer, size_t &total_transfer_size, size_t &endpoint_bytes_per_packet);
};

/**
*
*/
class VideoOutputStreamInterface : public Interface {
private:
protected:
public:
	VideoOutputStreamInterface(Device *device, const interface_descriptor_t *desc);
	virtual ~VideoOutputStreamInterface();
};

#endif /* UPC_INTERFACE_H_ */
