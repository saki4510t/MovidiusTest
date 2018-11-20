/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_TRANSFER_H_
#define UVC_TRANSFER_H_

// core
#include "core/transfer.h"
// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class UVCStream;

/**
*
*/
class UVCTransfer : public Transfer {
friend class UVCStream;
private:
	bool handle_transfer();
	void process_payload(UVCStream *stream, const uint8_t *payload, const size_t &payload_len);
	void process_payload_iso(UVCStream *stream);
	static void transfer_callback_func(Transfer *transfer);
protected:
public:
	// アイソクロナス転送用コンストラクタ
	UVCTransfer(Device *_device,
		const uint8_t &_endpoint,
		UVCStream *_stream,
		const size_t &iso_packets,
		const size_t &total_transfer_size,
		const size_t &endpoint_bytes_per_packet);
	// バルク/バルクストリーム転送用コンストラクタ
	UVCTransfer(Device *_device,
		const uint8_t &_endpoint,
		const uint32_t &_stream_id,
		UVCStream *_stream, const size_t &max_payload_transfer_size);
	virtual ~UVCTransfer();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_TRANSFER_H_ */
