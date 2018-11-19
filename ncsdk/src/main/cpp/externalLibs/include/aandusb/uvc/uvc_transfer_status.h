/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_TRANSFER_STATUS_H_
#define UVC_TRANSFER_STATUS_H_

// core
#include "core/transfer.h"
// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {
namespace uvc {

#define USE_STATUS_TRANSFER 1 // ステータス転送を使う時1

class UVCStream;

// dataがNULLだったりdata_lenが0になることがあるみたいなので注意
typedef void (uvc_status_callback_t)(enum uvc_status_class status_class,
		int event, int selector, enum uvc_status_attribute status_attribute,
		const uint8_t *data, size_t data_len, void *user_ptr);

#if USE_STATUS_TRANSFER
class StatusTransfer : public Transfer {
friend class UVCStream;
private:
	mutable Mutex callback_lock;
	uint8_t status_buf[32];
	uvc_status_callback_t *status_callback;
	void *status_user_ptr;
	// Transferイベントのエントリーポイント, static関数でないとダメ
	static void status_callback_func(Transfer *transfer);
	// status_callback_funcから呼び出される実際のイベントハンドラー
	bool handle_transfer();
protected:
	void handle_status_transfer();
	void handle_status_transfer_ctrl(const uint8_t *buffer, const int &actual_length);
	void handle_status_transfer_stream(const uint8_t *buffer, const int &actual_length);
public:
	StatusTransfer(Device *_device, const uint8_t &_endpoint);
	virtual ~StatusTransfer();
	void set_status_callback(uvc_status_callback_t *callback, void *user_ptr);
};
#endif

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_TRANSFER_STATUS_H_ */
