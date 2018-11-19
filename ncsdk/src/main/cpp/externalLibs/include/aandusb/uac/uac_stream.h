/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_STREAM_H_
#define UAC_STREAM_H_

#include <queue>
#include <vector>

#include "utilbase.h"
// uac
#include "aanduac.h"
#include "uac_frame.h"

namespace serenegiant {
namespace usb {
namespace uac {

// Transferの個数
#define UAC_NUM_TRANSFER_BUFS 10		// 1ミリ秒につき1パケット、10ミリ秒分
// Transferからの受信データ解析・保持バッファの最大サイズ
#define UAC_TRANSFER_BUF_SIZE	6144	// 32ビット✕384kHz✕2ch✕2倍

class UACDevice;
class UACDescriptor;
class AudioStreamInterface;
class UACTransfer;
class AudioFrame;

/**
 * 受信した音声フレーム毎に呼び出されるコールバック
 * キューイング済
 */
typedef void (*audio_stream_callback_t)(AudioFrame *frame, void *frame_callback_args);

/**
*
*/
class UACStream {
friend class UACTransfer;
private:
	uac_stream_ctrl currentControl;
	volatile bool running;			// 実行中フラグ
	volatile bool request_stop;		// 停止要求フラグ
	uint8_t bfh_err, hold_bfh_err;	// packet/transferのエラーフラグ
	uint32_t seq, hold_seq;			// transferのシーケンス番号
	uint32_t got_bytes, hold_bytes;	// 受信バイト数
	// active_frameとhold_frameは交互にswapして使う, バッファの実体へのポインタをoutbufが保持する
	uint8_t *outbuf;			// 受信したデータを保持するバッファ
	AudioFrame *active_frame;	// 受信用のバッファへのポインタ
	AudioFrame *hold_frame;		// バックバッファ
	// データの受信用
	std::vector<UACTransfer *> transfers;
	mutable Mutex transfers_lock;
	Condition transfers_lock_cond;
	// コールバック用
	volatile audio_stream_callback_t frame_callback;	// コールバック関数
	void *frame_callback_args;	// コールバック関数の引数
	AudioFrame callback_frame;	// コールバック用のフレームバッファ
	mutable Mutex _lock;
	Condition lock_cond;
	pthread_t stream_thread;
	//
	int start_streaming_iso(const uint8_t &endpoint_address, const uint32_t &packets_per_transfer, const uint32_t &total_transfer_size, const uint32_t &endpoint_bytes_per_packet);
	int start_streaming(const uac_stream_ctrl &ctrl);
	static void *stream_thread_func(void *args);
	void callback_loop();
	//
	void append(const uint8_t *payload, const size_t &length);
	void swap_buffers();
protected:
	UACDevice *device;
	UACDescriptor *descriptor;
	AudioStreamInterface *interface;
	uint32_t current_config_bytes_per_packet;
	uint8_t bInterfaceNumber;
	uint8_t bCtrlInterfaceNumber;
	int wIndexASEndpoint;
	void remove_transfer(UACTransfer *transfer);
	void release_transfers();
public:
	UACStream(AudioStreamInterface *interface, audio_stream_callback_t callback, void *usr_args);
	virtual ~UACStream();
	inline const bool is_running() const { return running && !request_stop; };
	inline const uac_stream_ctrl get_current_control() { return currentControl; };
	int find_stream(const uac_raw_format_t &format,
		const int &max_channels,		// 最大チャネル数, 0なら全てにマッチ
		const int &max_resolution,		// 解像度(ビット数), 0なら全てにマッチ
		const int &min_sampleing_freq,	// 最小サンプリング周波数
		const int &max_sampleing_freq);	// 最大サンプリング周波数
	int start();
	int stop();
	int check_stream();
	inline void lock() { _lock.lock(); };
	inline void unlock() { _lock.unlock(); };
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_STREAM_H_ */
