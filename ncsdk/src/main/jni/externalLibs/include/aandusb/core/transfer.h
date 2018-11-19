/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef TRANSFER_H_
#define TRANSFER_H_

#include "utilbase.h"
// core
#include "aandusb.h"
#include "internal.h"

namespace serenegiant {
namespace usb {

#define MAX_ISO_BUFFER_LENGTH		32768	// 32 x 1024 = 32KB
#define MAX_BULK_BUFFER_LENGTH		16384	// 16 x 1024 = 16KB
#define MAX_CTRL_BUFFER_LENGTH		4096	// 4 x 1024 = 4KB
#define MAX_BUST_NUM 16	// USB3.xでの最大バースト転送数

// 型名が長いので少し短く再定義
typedef struct usbdevfs_ctrltransfer ctrl_transfer_t;
typedef struct usbdevfs_bulktransfer bulk_transfer_t;

typedef enum transfer_type {
	TRANSFER_TYPE_UNKNOWN = 0,
	/** Control endpoint */
	TRANSFER_TYPE_CONTROL = 1,

	/** Isochronous endpoint */
	TRANSFER_TYPE_ISOCHRONOUS = 2,

	/** Bulk endpoint */
	TRANSFER_TYPE_BULK = 3,

	/** Interrupt endpoint */
	TRANSFER_TYPE_INTERRUPT = 4,

	/** Stream endpoint */
	TRANSFER_TYPE_BULK_STREAM = 5,
} transfer_type_t;

typedef enum transfer_status {
	/** エラー無し(要求した全てのデータが転送できたかどうかはわからない) */
	TRANSFER_STATUS_COMPLETED,
	/** 転送失敗 */
	TRANSFER_STATUS_ERROR,
	/** タイムアウト */
	TRANSFER_STATUS_TIMED_OUT,
	/** キャンセル */
	TRANSFER_STATUS_CANCELLED,
	/** バルク/インタラプト転送時にhalt(ストール)した
	 * またはコントロール転送時にサポートしてない機能を要求した */
	TRANSFER_STATUS_STALL,
	/** USB機器から切断された */
	TRANSFER_STATUS_NO_DEVICE,
	/** 転送量が要求したのよりも多い */
	TRANSFER_STATUS_OVERFLOW,
} transfer_status_t;

typedef enum internal_transfer_flags {
	/** タイムアウトした */
	TRANSFER_INTERNAL_FLAG_TIMED_OUT = 1 << 16,
	/** キャンセル処理中 */
	TRANSFER_INTERNAL_FLAG_CANCELLING = 1 << 17,
	/** USB機器が取り外されて転送に失敗した */
	TRANSFER_INTERNAL_FLAG_DEVICE_DISAPPEARED = 1 << 18,
} internal_transfer_flags_t;

typedef enum transfer_request_flags {
	/** short frameをエラーとして扱うかどうか */
	TRANSFER_REQ_SHORT_NOT_OK = 1 << 0,
	/** 破棄時にbufferフィールドをfreeする */
	TRANSFER_REQ_FREE_BUFFER = 1 << 1,
	/** 破棄時にbufferフィールドをdeleteする */
	TRANSFER_REQ_DELETE_BUFFER = 1 << 2,
	/** 破棄時にbufferフィールドをdelete[]する */
	TRANSFER_REQ_DELETE_BUFFER_ARRAY = 1 << 3,
	/** このフラグが立っていれば転送完了時に自動的に破棄される */
	TRANSFER_REQ_FREE_TRANSFER = 1 << 4,
	/** ゼロレングスパケット */
	TRANSFER_REQ_ADD_ZERO_PACKET = 1 << 5,
} transfer_request_flags_t;

typedef enum reap_action {
	/** 正常終了 */
	REAP_ACTION_NORMAL = 0,
	/** 最初のURBのsubmissionが失敗した。残りがキャンセルか完了するまで待たないとダメ */
	REAP_ACTION_SUBMIT_FAILED,
	/** キャンセルした */
	REAP_ACTION_CANCELLED,
	/** URB転送完了(でもまだ最後のURBまでは転送できてない) */
	REAP_ACTION_COMPLETED_EARLY,
	/** なんかわからんエラーになってもうた */
	REAP_ACTION_ERROR,
} reap_action_t;

class Context;
class Device;
class Transfer;

/** transferコールバック関数プロトタイプ */
typedef void (*transfer_callback_t)(Transfer *Transfer);

/**
*
*/
class Transfer {
friend Context;
private:
	const uint32_t _id;
	/** エンドポイントの種類 */
	const transfer_type_t transfer_type;
	/** タイムアウト[ミリ秒], 0なら無限待ち */
	const unsigned int timeout_millis;
	// バルクストリーム転送時のストリームid
	const uint32_t stream_id;
	/** このtransferの送り先のエンドポイントアドレス */
	const unsigned char endpoint;
	/** コンテキスト */
	Context *context;

	enum reap_action reap_action;
	// transferの結果
	// アイソクロナス転送の時は失敗しててもCOMPLETEDになることが有るので、
	// 代わりに各usbdevfs_iso_packet_desc内のstatusを使うこと
	transfer_status_t reap_status;
	size_t transferred;
	int num_urbs;
	int num_completed;
	int iso_packet_offset;
	/** フラグビット */
	volatile uint32_t internal_flags;
	/** 転送するデータバッファ */
	uint8_t *buffer;
	/** データバッファの確保してあるバイト数 */
	const size_t buffer_length;
	/** データバッファの有効バイト数(コールバック関数内でのみ有効)
	 * アイソクロナス転送の場合は無効なので、各usbdevfs_iso_packet_desc内の
	 * actual_lengthを使うこと */
	size_t actual_length;

	/**
	 * 他のコンストラクタの本体
	 */
	Transfer(Device *_device,
		const transfer_type_t _transfer_type,
		const uint8_t &_endpoint,
		const uint32_t &_stream_id,
		const size_t &iso_packets,
		unsigned char *_buffer,
		const size_t &_length,
		transfer_callback_t _callback,
		void *_user_data,
		const unsigned int &_timeout_millis);

	int calculate_timeout();
	int discard_urbs(const int &first, const int &last_plus_one);
	void free_urbs_locked();
	int submit_bulk_transfer();
	int submit_iso_transfer();
	int submit_control_transfer();
	int reap_completed(transfer_status_t status);
	int reap_canceled();
	int reap_bulk_transfer(struct usbdevfs_urb *urb);
	int reap_iso_transfer(struct usbdevfs_urb *urb);
	int reap_control_transfer(struct usbdevfs_urb *urb);
protected:
	/** アイソクロナスのパケット数(アイソクロナス転送の時のみ有効) */
	const size_t num_iso_packets;
	/** transferを受け取るUSB device */
	Device *device;
	mutable Mutex lock;
	/** 転送中フラグ */
	bool running;
	volatile uint8_t req_flags;
	/** タイムアウトチェック用の変数 */
	struct timeval timeout;
	/** コールバック関数
	 * 転送完了、転送失敗、転送キャンセル時に呼び出される */
	transfer_callback_t callback;
	/** コールバック関数で使用するユーザーデータ */
	void *user_data;
	/** アイソクロナス転送用のdescriptor(アイソクロナス転送の時のみ有効) */
	struct usbdevfs_iso_packet_desc *iso_packet_desc;
	union {
		struct usbdevfs_urb *urbs;
		struct usbdevfs_urb **iso_urbs;
	};

	uint8_t *get_iso_packet_buffer(const size_t &packet);
	void dump_transfer();
	void dump_urb(int ix, int fd, struct usbdevfs_urb *urb);
	// Contextから呼ばれる
	bool check_timeout();
	void handle_timeout();
	// これはスタティック
	static int reap(Device *device);
public:
	/** コントロール転送用コンストラクタ */
	Transfer(Device *_device,
		ctrl_transfer_t *_buffer,
		transfer_callback_t _callback, void *_user_data,
		const unsigned int &_timeout_millis);
	/**
	 * バルク/バルクストリーム転送用コンストラクタ
	 * (_stream_idが0ならbulk transfer, 1以上ならbulk stream transfer)
	 */
	Transfer(Device *_device, const uint8_t &_endpoint,
		const uint32_t &_stream_id,
		unsigned char *_buffer, const size_t &_length,
		transfer_callback_t _callback, void *_user_data,
		const unsigned int &_timeout_millis);
	/** インタラプト転送用コンストラクタ */
	Transfer(Device *_device, const uint8_t &_endpoint,
		unsigned char *_buffer, const size_t &_length,
		transfer_callback_t _callback, void *_user_data,
		const unsigned int &_timeout_millis);
	/** アイソクロナス転送用コンストラクタ */
	Transfer(Device *_device, const uint8_t &_endpoint,
		unsigned char *_buffer, const size_t &_length,
		const size_t &_num_iso_packets, const size_t &endpoint_bytes_per_packet,
		transfer_callback_t _callback, void *_user_data,
		const unsigned int &_timeout_millis);

	virtual ~Transfer();

	inline Context *get_context() { return context; };
	inline const uint32_t id() const { return _id; };
	inline const transfer_status_t &get_status() { return reap_status; };
	inline const unsigned char &get_endpoint() { return endpoint; };
	inline const uint8_t *get_buffer() { return buffer; };
	inline const size_t &get_actual_length() { return actual_length; };
	inline struct timeval &get_timeout() { return timeout; };
	inline const bool is_running() const { return urbs && running; };
	inline const bool is_cancelling() const {
		return (internal_flags & TRANSFER_INTERNAL_FLAG_CANCELLING) == TRANSFER_INTERNAL_FLAG_CANCELLING; };
	virtual int submit();
	virtual int cancel();
	virtual void release();
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* TRANSFER_H_ */
