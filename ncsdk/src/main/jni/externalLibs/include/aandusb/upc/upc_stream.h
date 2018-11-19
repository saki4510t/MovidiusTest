/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_STREAM_H_
#define UPC_STREAM_H_

#include "utilbase.h"
// core
#include "core/aandusb.h"
#include "cpre/internal.h"
// upc
#include "androupc.h"
#include "upc_frame.h"

// Transferの個数
#define NUM_TRANSFER_BUFS 5	// 10 大きすぎても小さすぎても動かない
// Transferからの受信データ解析・保持バッファの最大サイズ
#define TRANSFER_BUF_SIZE	(10 * 1024 * 1024 )	// 最大映像サイズが2592x1944でYUYVで受信すれば2592x1944x2≒9.61x1024x1024

class UPCDevice;
class UPCDescriptor;
class VideoInputStreamInterface;
class UPCTransfer;
class UPCVideoFrame;

// 受信した映像フレーム毎に呼び出されるコールバック
typedef void (*video_stream_callback_t)(UPCVideoFrame *frame, void *frame_callback_args);

/**
*
*/
class UPCStream {
friend class UPCTransfer;
private:
	volatile bool running;
	volatile bool request_stop;
	uint8_t payload_header, hold_payload_header;
	uint8_t bfh_err, hold_bfh_err;
	uint8_t fid;
	uint32_t seq, hold_seq;
	uint32_t pts, hold_pts;
	uint32_t last_scr, hold_last_scr;
	uint32_t got_bytes, hold_bytes;
	// active_frameとhold_frameは交互にswapして使う, バッファの実体へのポインタをoutbufが保持する
	uint8_t *outbuf;		// 受信したデータを保持するバッファ
	UPCVideoFrame *active_frame;
	UPCVideoFrame *hold_frame;
	UPCVideoFrame callback_frame;	// コールバック用にhold_frameからcallback_frameへコピーする
	//
	std::vector<UPCTransfer *> transfers;
	mutable Mutex transfers_lock;
	Condition transfers_lock_cond;
	//
	volatile video_stream_callback_t frame_callback;
	void *frame_callback_args;
	mutable Mutex _lock;
	Condition lock_cond;
	pthread_t stream_thread;
	//
	int start_streaming_iso(const uint8_t &endpoint_address, const uint32_t &packets_per_transfer, const uint32_t &total_transfer_size, const uint32_t &endpoint_bytes_per_packet);
	int start_streaming(const uint8_t &endpoint_address, const uint32_t &config_bytes_per_packet, const uint32_t &dwMaxUPCVideoFrameSize);
	static void *stream_thread_func(void *args);
	void event_loop();
	void prepare_callback_frame();
	//
	void append(const uint8_t *payload, const size_t &length);
	void swap_buffers();
protected:
	UPCDevice *device;
	//

	VideoInputStreamInterface *interface;
	uint32_t current_config_bytes_per_packet;
	uint8_t bInterfaceNumber;
	uint8_t bCtrlInterfaceNumber;
	int wIndexVSEndpoint;
	UPCDescriptor *descriptor;
	uint32_t width, height;
	upc_frame_format_t frame_format;
	int select_stream_uncompressed(const int width, const int height, const int min_fps, const int max_fps);
	void remove_transfer(UPCTransfer *transfer);
	void release_transfers();
	int internal_negotiateAltSetting(uint32_t &packets_per_transfer, uint32_t &total_transfer_size,
			uint32_t &endpoint_bytes_per_packet, const uint32_t &dwMaxVideoFrameSize);
	int negotiateAltSetting(const bool &reset
		const uint32_t &config_bytes_per_packet, const uint32_t &max_vide_frame_szie,
		uint32_t &packets_per_transfer, uint32_t &total_transfer_size, uint32_t &endpoint_bytes_per_packet);
public:
	UPCStream(VideoInputStreamInterface *interface, video_stream_callback_t callback, void *usr_args);
	virtual ~UPCStream();
	inline const bool isRunning() const { return running && !request_stop; };
	int find_stream(const upc_raw_frame &frame_type,
		const uint32_t &width, const uint32_t &height,
		const int &min_fps, const int &max_fps);
	int start();
	int stop();
	inline void lock() { _lock.lock(); };
	inline void unlock() { _lock.unlock(); };
};

#endif /* UPC_STREAM_H_ */
