/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_INTERFACE_H_
#define UAC_INTERFACE_H_

// core
#include "core/interface.h"
// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {

class Config;

namespace uac {

/**
*
*/
class AudioControlInterface : public Interface {
private:
	const uac1_ac_header_descriptor_t *header_descriptor;
protected:
public:
	AudioControlInterface(Device *device, Config *config, const interface_descriptor_t *desc);
	virtual ~AudioControlInterface();
	void set_header_descriptor(const uac1_ac_header_descriptor_t *_header_descriptor) {
		header_descriptor = _header_descriptor;
	}
	const uac1_ac_header_descriptor_t *getHeaderDescriptor() { return header_descriptor; };
};

/**
*
*/
class AudioStreamInterface : public Interface {
private:
	const uac1_as_header_descriptor_t *header_descriptor;
	uint8_t bEndpointAddress;
protected:
	int direction;	// USB_DIR_OUT/USB_DIR_IN
	int get_sampling_frequency(const uint8_t &endpoint_address, uint32_t &sampling_freqency, req_code_t req_code);
	int set_sampling_frequency(const uint8_t &endpoint_address, const uint32_t &sampling_freqency);
public:
	AudioStreamInterface(Device *device, Config *config, const interface_descriptor_t *desc);
	virtual ~AudioStreamInterface();
	inline void set_header_descriptor(const uac1_as_header_descriptor_t *_header_descriptor) {
		header_descriptor = _header_descriptor;
	}
	inline const uac1_as_header_descriptor_t *get_header_descriptor() { return header_descriptor; };
	inline void set_direction(const int _direction) { direction = _direction; };
	inline void set_endpoint_address(const uint8_t &endpointaddress) { bEndpointAddress = endpointaddress; };
	inline const uint8_t &get_endpoint_address() const { return bEndpointAddress; };
	int find_alt_setting(const uac_raw_format_t &format,
		const int &max_channels,		// 最大チャネル数, 0なら全てにマッチ
		const int &max_resolution,		// 解像度(ビット数), 0なら全てにマッチ
		const int &min_sampling_freq,	// 最小サンプリング周波数
		const int &max_sampling_freq,	// 最大サンプリング周波数)
		uac_stream_ctrl &ctrl);
	int commit(const uac_stream_ctrl &ctrl);

#if 0
	int findAltSetting(const size_t &dwMaxVideoFrameSize, const size_t &bytes_per_packet,
		Endpoint *&result, size_t &packets_per_transfer, size_t &total_transfer_size, size_t &endpoint_bytes_per_packet);
#endif
	void dump_stream_ctrl(const uac_stream_ctrl &ctrl);
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_INTERFACE_H_ */
