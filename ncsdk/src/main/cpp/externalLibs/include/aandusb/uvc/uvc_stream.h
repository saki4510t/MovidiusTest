/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_STREAM_H_
#define UVC_STREAM_H_

#include "utilbase.h"
// core
#include "core/aandusb.h"
#include "core/internal.h"
// uvc
#include "aanduvc.h"
#include "uvc_frame.h"
#include "UvcH264.h"

namespace serenegiant {
namespace usb {
namespace uvc {

// Transferからの受信データ解析・保持バッファの最大サイズ
#define TRANSFER_BUF_SIZE	(10 * 1024 * 1024 )	// 最大映像サイズが2592x1944でYUYVで受信すれば2592x1944x2≒9.61x1024x1024

class UVCDevice;
class UVCDescriptor;
class VideoInputStreamInterface;
class UVCTransfer;
class VideoFrame;

/**
*
*/
class UVCStream {
friend class UVCTransfer;
private:
	const uint32_t num_transfer_bufs;
	volatile bool running;
	volatile int stalled;
	volatile bool request_stop;
	volatile bool still_capture;
	uint8_t bStreamInterfaceNumber;
	uint8_t bCtrlInterfaceNumber;
	uint8_t payload_header, hold_payload_header;
	uint8_t bfh_err, hold_bfh_err;
	int8_t fid;
	uint32_t seq, hold_seq;
	uint32_t pts;
	uint32_t last_stc;	// stc+sof==scr
	uint32_t last_sof;
	size_t got_bytes, hold_bytes;
	// ペイロード中のマイクロフレームの数
	int frame_num_in_payload;
	// active_frameとhold_frameは交互にswapして使う, バッファの実体へのポインタをoutbufが保持する
	uint8_t *outbuf;		// 受信したデータを保持するバッファ
	VideoFrame *active_frame;
	VideoFrame *hold_frame;
	VideoFrame callback_frame;	// コールバック用にhold_frameからcallback_frameへコピーする
	VideoFrame aux_frame;		// 外部ストリームデータの取り出し用
	//
	std::vector<UVCTransfer *> transfers;
	mutable Mutex transfers_lock;
	Condition transfers_lock_cond;
	//
	void *frame_callback_args;
	video_stream_callback_t frame_callback;
	video_stream_callback_t frame_callback_aux;
	mutable Mutex _lock;
	Condition lock_cond;
	pthread_t stream_thread;
	// 静止画キャプチャのコールバック(メソッド2または3の時)
	volatile still_capture_callback_t still_callback;
	void *still_callback_args;
	//
	int start_transfer_iso(const uint8_t &endpoint_address,
		const uint32_t &packets_per_transfer,
		const uint32_t &total_transfer_bytes,
		const uint32_t &endpoint_max_packet_bytes);
	int start_transfer_bulk(const uint8_t &endpoint_address,
		const uint32_t &packets_per_transfer,
		const uint32_t &total_transfer_bytes,
		const uint32_t &endpoint_max_packet_bytes);
	/**
	 * ストリーミング開始処理
	 * @param endpoint_address
	 * @param max_payload_transfer_bytes 要求帯域(1ペイロードあたりの最大転送バイト数)
	 * @param max_video_frame_size 1フレームあたりの最大フレームバイト数
	 */
	int start_streaming(const uint8_t &endpoint_address,
		const uint32_t &max_payload_transfer_bytes,
		const uint32_t &max_video_frame_size);
	/**
	 * ストリーミング開始処理, FULL_BANDWIDTH_NEGOTIATION用
	 * @param endpoint_address
	 * @param max_payload_transfer_bytes 要求帯域(1ペイロードあたりの最大転送バイト数)
	 * @param max_video_frame_size 1フレームあたりの最大フレームバイト数
	 */
	int start_streaming2(const uint8_t &endpoint_address,
		const size_t &max_payload_transfer_bytes,
		const uint32_t &max_video_frame_size);
	int start_capture(const uint8_t &endpoint_address);
	static void *stream_thread_func(void *args);
	void event_loop();
	void prepare_callback_frame();
	//
	void append(const uint8_t *payload, const size_t &length);
	void swap_buffers(const size_t &packet_id = 0);
protected:
	UVCDevice *device;
	//
	streaming_control_t currentControl;
	streaming_control_t minControl;
	streaming_control_t maxControl;
	streaming_control_t defControl;
	//
	video_still_control_t currentStillControl;
	video_still_control_t minStillControl;
	video_still_control_t maxStillControl;
	video_still_control_t defStillControl;

	// 多重化されている外部ストリーム用
	uvcx_video_config_probe_commit_t currentH264Config;	// 本来はh264限定では無いけど、今は実質h264専用
	uint8_t bStreamMuxOption;
	uint8_t bAuxUnitNumber;
	uint8_t bStreamID;
	raw_frame_t aux_host_frame_type;	// 多重化されている時にフレームデータを保持している映像フレームのフォーマット(RAW_FRAME_MJPEG)
	bool is_multi_stream;
	//

	VideoInputStreamInterface *interface;
	float bandwidth_factor;
	uint8_t current_endpoint_address;
	size_t current_max_payload_transfer_bytes;
	uint32_t current_clock_frequency;
	uint32_t current_packets_per_transfer;
	int wIndexVSEndpoint;
	UVCDescriptor *descriptor;
	uint32_t width, height;
	raw_frame_t frame_type;
	uint32_t still_capture_width, still_capture_height;
	int query_ctrl(streaming_control_t &ctrl, const bool &probe, const req_code_t &req);
	int query_ctrl(video_still_control_t &ctrl, const bool &probe, const req_code_t &req);
	int prepare_ctrl(const bool &log = false);
	int prepare_still_ctrl(const bool &log = false);
	int probe_ctrl(streaming_control_t &ctrl);
	int probe_ctrl(video_still_control_t &ctrl);
	int probe_ctrl_aux(const uint8_t &unit_no, uvcx_video_config_probe_commit_t &_config);
	inline int commit_ctrl(streaming_control_t &_ctrl);
	inline int commit_ctrl(video_still_control_t &_ctrl);
	inline int commit_ctrl_aux(const uint8_t &unit_no,
		uvcx_video_config_probe_commit_t &_config);
	int start_transfer(const bool &is_iso, const uint8_t &endpoint_address,
		const uint32_t &packets_per_transfer,
		const uint32_t &total_transfer_bytes,
		const uint32_t &endpoint_max_packet_bytes);
	void remove_transfer(UVCTransfer *transfer);
	void release_transfers();
	/**
	 * 要求帯域にマッチする代替設定を探して見つかればその代替設定を選択する
	 * negotiate_alt_settingの下請け
	 *
	 * @param max_payload_transfer_bytes 要求帯域(1ペイロードあたりの最大転送バイト数)
	 * @param max_video_frame_size 1フレームあたりの最大フレームバイト数
	 * @out packets_per_transfer 1パケットあたりの転送バイト数
	 * @out total_transfer_bytes 1回あたりの想定転送バイト数
	 * @out endpoint_max_packet_bytes 見つかったエンドポイントの最大パケットバイト数
	 */
	int internal_negotiate_alt_setting(
		const size_t &max_payload_transfer_bytes, const uint32_t &max_video_frame_size,
		uint32_t &packets_per_transfer,
		uint32_t &total_transfer_size,
		uint32_t &endpoint_bytes_per_packet);
	/**
	 * 要求帯域にマッチする代替設定を探して見つかればその代替設定を選択する
	 *
	 * @param max_payload_transfer_bytes 要求帯域(1ペイロードあたりの最大転送バイト数)
	 * @param max_video_frame_size 1フレームあたりの最大フレームバイト数
	 * @out packets_per_transfer 1パケットあたりの転送バイト数
	 * @out total_transfer_bytes 1回あたりの想定転送バイト数
	 * @out endpoint_max_packet_bytes 見つかったエンドポイントの最大パケットバイト数
	 */
	int negotiate_alt_setting(
		const size_t &max_payload_transfer_bytes, const uint32_t &max_video_frame_size,
		uint32_t &packets_per_transfer,
		uint32_t &total_transfer_size,
		uint32_t &endpoint_bytes_per_packet);
	int setup_h264_config(const uint8_t &unit_no,
		const uint8_t &bStreamMuxOption,
		const uint32_t &width, const uint32_t &height,
		const bool &commit);
public:
	UVCStream(VideoInputStreamInterface *interface,
		void *usr_args, video_stream_callback_t callback,
		video_stream_callback_t callback_aux = NULL,
		const uint32_t &num_transfer_bufs = NUM_TRANSFER_BUFS,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE);
	virtual ~UVCStream();
	inline const bool is_running() const { return running && !request_stop; };
	const inline streaming_control_t& get_current() { return currentControl; };
	const inline uvcx_video_config_probe_commit_t& get_current_aux() { return currentH264Config; };
	const inline uint32_t &clock_frequency() const { return current_clock_frequency; };
	const inline uint32_t &packets_per_transfer() const { return current_packets_per_transfer; };
	inline const uint8_t stream_interface_number() const { return bStreamInterfaceNumber; };
	inline const uint8_t ctrl_interface_number() const { return bCtrlInterfaceNumber; };
	int find_stream(const raw_frame &frame_type,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps, const float &bandwidth_factor = 0.0f);
	int find_stream2(const raw_frame &frame_type,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps, const float &bandwidth_factor = 0.0f);
	int find_capture(const uint32_t &width, const uint32_t &height, uint8_t &endpoint_address);
	int find_stream_aux(const raw_frame &frame_type,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps, const float &bandwidth_factor = 0.0f);
	int start();
	int start2();
	int stop();
	int start_aux();
	int stop_aux();
	inline void lock() { _lock.lock(); };
	inline void unlock() { _lock.unlock(); };
	int take_still_capture(const uint32_t &width, const uint32_t &height,
		still_capture_callback_t callback, void *usr_args,
		const bool &enable_capture_method = false);
	const still_capture_method_t get_still_capture_method(const bool &enable_capture_method = false);
	int get_vs_error_code(uint8_t &error_code, req_code_t req_code);
	inline vs_error_code_control_t get_vs_error_code() {
		uint8_t error_code;
		if (!get_vs_error_code(error_code, REQ_GET_CUR))
			return (vs_error_code_control_t)error_code;
		else
			return VS_ERROR_CODECTRL_ERROR;
	}
	int get_vc_error_code(uint8_t &error_code, req_code_t req_code);
	inline vc_error_code_control_t get_vc_error_code() {
		uint8_t error_code;
		if (!get_vc_error_code(error_code, REQ_GET_CUR))
			return (vc_error_code_control_t)error_code;
		else
			return VC_ERROR_CODECTRL_ERROR;
	}
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_STREAM_H_ */
