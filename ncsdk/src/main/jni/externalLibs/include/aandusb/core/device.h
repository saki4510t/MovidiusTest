/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <sys/ioctl.h>

#include "utilbase.h"
// core
#include "aandusb.h"
#include "internal.h"
#include "config.h"

namespace serenegiant {
namespace usb {

class Context;
class Descriptor;
class Interface;
class Transfer;

/**
*
*/
class Device {
friend class Context;
friend class Interface;
private:
    /** ディスクリプタ */
    uint8_t desc[4096];
    /** ディスクリプタのバイト数 */
	size_t desc_length;
    int fd;
    /** USB機器のcapabilityフラグ */
	uint32_t caps;
	/** 現在選択されているコンフィグレーション, 未選択なら-1 */
	int active_config;
	/** 排他制御用のミューテックス */
	mutable Mutex device_lock;
	std::list<Transfer *> transfers;
	std::vector<unsigned int> claimed_interface;	// FIXME これもConfig毎に保持しないとダメかも
	/** 参照カウンタ */
	int refCount;
	/**
	 * エンドポイントへのストリームの割り当て・開放処理の実体
	 * @param request USBDEVFS_ALLOC_STREAMSまたはUSBDEVFS_FREE_STREAMS
	 * @param num_streams ストリームの数, 開放時は0
	 * @param endpoints エンドポイント配列
	 * @param num_endpoints エンドポイントの数
	 * @return 負ならエラー, 正なら割り当てた数
	 */
	int handle_stream_request(const int &request,
		const uint32_t &num_streams,
		const uint8_t *endpoints, const size_t &num_endpoints);
protected:
	Context *context;
	Descriptor *descriptor;
public:
	/**
	 * コンストラクタ
	 */
	Device(Context *_context, Descriptor *_descriptor, const int &fd);
	/**
	 * デストラクタ
	 */
	virtual ~Device();
	/**
	 * コンテキストオブジェクトを取得
	 */
	inline Context *get_context() { return context; };
	/**
	 * ファイルディスクリプタを取得
	 */
	inline int &get_fd() { return fd; };
	/**
	 * openしているかどうかを取得
	 */
	inline const bool is_active() const { return fd != 0; };
	/**
	 * USB機器のcapabilityフラグを取得
	 */
	inline uint32_t &get_caps() { return caps; };
	/**
	 * ディスクリプタを取得
	 */
	inline Descriptor *get_descriptor() { return descriptor; };
	/**
	 * rawディスクリプタをバイト配列としてコピーする
	 * bufferにnullを渡すとディスクリプタのサイズを取得できる
	 */
	int get_raw_descriptor(uint8_t *buffer, size_t *descriptors_len, int *host_endian);
	/**
	 * rawディスクリプタ配列の先頭ポインタを取得
	 */
	inline const uint8_t *get_raw_descriptor() const { return &desc[0]; };
	/**
	 * rawディスクリプタの長さ(バイト数)を取得
	 */
	inline const size_t get_raw_descriptor_length() const { return desc_length; };
	/**
	 * ベンダーIDを首都kする
	 */
	uint16_t get_vendor_id();
	/**
	 * プロダクトIDを取得する
	 */
	uint16_t get_product_id();
	/**
	 * デバイスディスクリプタ構造体を取得する
	 */
	const device_descriptor_t *get_device_descriptor();
	/**
	 * ストリングディスクリプタから指定したidの文字列を取得する
	 * 見つからなければNULLを返す。
	 * NULL以外が返った時にはfreeすること
	 */
	char *get_string(int id);
	/**
	 * マニファクチャ名文字列を取得する
	 * 見つからなければNULLを返す。
	 * NULL以外が返った時にはfreeすること
	 */
	char *get_manufacturer_name();
	/**
	 * プロダクト名文字列を取得する
	 * 見つからなければNULLを返す。
	 * NULL以外が返った時にはfreeすること
	 */
	char *get_product_name();
	/**
	 * シリアル番号文字列を取得する
	 * 見つからなければNULLを返す。
	 * NULL以外が返った時にはfreeすること
	 */
	char *get_serial();

	/**
	 * 参照カウンタをインクリメント
	 * (今のところ特にそれ以外の動作は無し)
	 */
	int ref();
	/**
	 * 参照カウンタをデクリメント
	 * (今のところ特にそれ以外の動作は無し)
	 */
	int unref();
	/**
	 * 排他制御用のミューテックスをロック
	 */
	int lock();
	/**
	 * 排他制御用のミューテックスをロック解除
	 */
	void unlock();
	/**
	 * 指定したインターフェースへ接続する
	 */
	int claim_interface(unsigned int interface);
	/**
	 * 指定したインターフェースとの接続を解除
	 */
	int release_interface(unsigned int interface, const bool &force = false);
	/**
	 * 指定したインターフェースと接続しているかどうか(内部フラグを参照するだけ)
	 */
	int is_claimed(unsigned int interface);

	/**
	 * 全てのインターフェースを開放して機器をリセット
	 */
	int reset();
	/**
	 * USB機器をリセットする
	 * reset前にclaimされていたインターフェースは一旦開放された後、再claimを試みる
	 */
	int reset_and_claim();
	/**
	 * カーネルドライバの接続要求/切断要求をする
	 * カーネルドライバが接続したままだとclaim出来ない
	 */
	int connect_kernel_driver(unsigned int interface, int connect);
	/**
	 * 指定したインターフェースの代替設定を設定
	 */
	int set_interface(unsigned int interface, unsigned int alt_setting);
	/**
	 * 現在選択されているコンフィギュレーション(番号)を取得する
	 */
	int get_active_config();
	/**
	 * 指定したコンフィギュレーションを選択
	 */
	int set_configuration(unsigned int configurationValue);
	/**
	 * 指定したコンフィギュレーションを選択
	 */
	inline int set_configuration(Config *config) {
		return set_configuration(config ? config->configuration_value() : 1);
	};
	/**
	 * halt(stall)状態を解除要求
	 */
	int clear_halt(uint8_t endpoint);

	/**
	 * コントロール転送実行(同期処理)
	 * 指定したデバイスのエンドポイント0へコントロールメッセージを送る
	 */
	int control_transfer_sync(const uint8_t &bRequestType, const uint8_t &bRequest,
		const int &wValue,
		const int &wIndex,
		void *data, const int &length, const uint32_t &timeout);

	/**
	 * バルク転送実行(同期処理)
	 * バルクエンドポイントの読み書き実行
	 * @return 転送したバイト数またはエラーコード(負数)を返す
	 */
	int bulk_transfer_sync(int bEndpoint, void *data, int length, uint32_t timeout);

	/**
	 * バルクストリーム転送用にエンドポイントにストリームを割り当てる
	 * @param num_streams
	 * @param endpoints ストリームを割り当てるエンドポイント配列
	 * @param num_endpoints ストリームを割り当てるエンドポイントの数
	 * @return 負ならエラー、正なら割り当てたストリームの数
	 */
	inline int allocate_streams(const uint32_t &num_streams,
		const uint8_t *endpoints, const size_t &num_endpoints) {
	
		return handle_stream_request(USBDEVFS_ALLOC_STREAMS,
			num_streams, endpoints, num_endpoints);
	}
	
	/**
	 * エンドポイントに割り当てたストリームを開放する
	 * @param endpoints ストリームを割り当てたエンドポイント配列
	 * @param num_endpoints ストリームを割り当てたエンドポイントの数
	 * @return 負ならエラー
	 */
	inline int free_streams(
		const uint8_t *endpoints, const size_t &num_endpoints) {

		return handle_stream_request(USBDEVFS_FREE_STREAMS,
			0, endpoints, num_endpoints);
	}
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* DEVICE_H_ */
