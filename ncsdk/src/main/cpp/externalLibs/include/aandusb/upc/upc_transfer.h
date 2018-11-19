/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_TRANSFER_H_
#define UPC_TRANSFER_H_

// core
#include "core/transfer.h"
// upc
#include "androupc.h"

#define USE_STATUS_TRANSFER 0

class UPCStream;

/**
*
*/
class UPCTransfer : public Transfer {
friend class UPCStream;
private:
	bool handle_transfer();
	void process_payload(UPCStream *strm, const uint8_t *payload, const size_t &payload_len);
	void process_payload_iso(UPCStream *stream);
	static void transfer_callback_func(Transfer *transfer);
protected:
public:
	// アイソクロナス転送用コンストラクタ
	UPCTransfer(Device *_device, const uint8_t &_endpoint, UPCStream *_stream,
		const int &iso_packets, const size_t &total_transfer_size, const size_t &endpoint_bytes_per_packet);
	// バルク転送用コンストラクタ
	UPCTransfer(Device *_device, const uint8_t &_endpoint, UPCStream *_stream, const size_t &dwMaxPayloadTransferSize);
	virtual ~UPCTransfer();
};

#endif /* UPC_TRANSFER_H_ */
