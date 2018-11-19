/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_TRANSFER_H_
#define UAC_TRANSFER_H_

// core
#include "core/transfer.h"
// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {

class Device;

namespace uac {

class UACStream;

/**
*
*/
class UACTransfer: public Transfer {
friend class UACStream;
private:
	bool handle_transfer();
	void process_payload(UACStream *strm, const uint8_t *payload, const size_t &payload_len);
	void process_payload_iso(UACStream *stream);
	static void transfer_callback_func(Transfer *transfer);
protected:
public:
	// アイソクロナス転送用コンストラクタ
	UACTransfer(Device *_device, const uint8_t &_endpoint, UACStream *_stream,
		const int &iso_packets, const size_t &total_transfer_size, const size_t &endpoint_bytes_per_packet);
	virtual ~UACTransfer();
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_TRANSFER_H_ */
