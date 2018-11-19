/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_INTERFACE_H_
#define UVC_INTERFACE_H_

// core
#include "core/interface.h"
// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {

class Config;

namespace uvc {

/**
*
*/
class VideoControlInterface : public Interface {
private:
	const uvc_header_descriptor_t *header_descriptor;
protected:
public:
	VideoControlInterface(Device *device, Config *config, const interface_descriptor_t *desc);
	virtual ~VideoControlInterface();
	void set_header_descriptor(const uvc_header_descriptor_t *_header_descriptor) {
		header_descriptor = _header_descriptor;
	}
	const uvc_header_descriptor_t *get_header_descriptor() { return header_descriptor; };
	const uint32_t clock_frequency() const;
};

/**
*
*/
class VideoInputStreamInterface : public Interface {
private:
	const input_header_descriptor_t *header_descriptor;
protected:
public:
	VideoInputStreamInterface(Device *device, Config *config, const interface_descriptor_t *desc);
	virtual ~VideoInputStreamInterface();
	inline void set_header_descriptor(const input_header_descriptor_t *_header_descriptor) {
		header_descriptor = _header_descriptor;
	}
	inline const input_header_descriptor_t *get_header_descriptor() { return header_descriptor; };
	/**
	 * 要求帯域にマッチする代替設定を探す
	 *
	 * @param max_payload_transfer_bytes 要求帯域(1ペイロードあたりの最大転送バイト数)
	 * @param max_video_frame_size 1フレームあたりの最大フレームバイト数
	 * @out packets_per_transfer 1パケットあたりの転送バイト数
	 * @out total_transfer_bytes 1回あたりの想定転送バイト数
	 * @out endpoint_max_packet_bytes 見つかったエンドポイントの最大パケットバイト数
	 */
	int find_alt_setting(
		const size_t &max_payload_transfer_size,
		const uint32_t &dwMaxVideoFrameSize,
		Endpoint *&result,
		uint32_t &packets_per_transfer,
		uint32_t &total_transfer_size,
		uint32_t &endpoint_max_packet_bytes);
};

/**
*
*/
class VideoOutputStreamInterface : public Interface {
private:
	const output_header_descriptor_t *header_descriptor;
protected:
public:
	VideoOutputStreamInterface(Device *device, Config *config, const interface_descriptor_t *desc);
	virtual ~VideoOutputStreamInterface();
	inline void set_header_descriptor(const output_header_descriptor_t *_header_descriptor) {
		header_descriptor = _header_descriptor;
	}
	inline const output_header_descriptor_t *get_header_descriptor() { return header_descriptor; };
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_INTERFACE_H_ */
