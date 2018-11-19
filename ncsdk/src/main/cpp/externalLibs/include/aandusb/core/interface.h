/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

// core
#include "aandusb.h"
#include "internal.h"

namespace serenegiant {
namespace usb {

typedef enum interface_type {
	INTERFACE_UNKNOWN = 0,
	// UVC
	INTERFACE_UVC_VIDEO_CONTROL = 1001,
	INTERFACE_UVC_INPUT_STREAM = 1002,
	INTERFACE_UVC_OUTPUT_STREAM = 1003,
	// UAC
	INTERFACE_UAC_AUDIO_CONTROL = 2001,
	INTERFACE_UAC_AUDIO_INPUT_STREAM = 2002,
	INTERFACE_UAC_AUDIO_OUTPUT_STREAM = 2003,
	INTERFACE_UAC_MIDI_INPUT_STREAN = 2004,
	INTERFACE_UAC_MIDI_OUTPUT_STREAN = 2005,
} interface_type_t;

class Device;
class Config;
class Endpoint;

/**
*
*/
class Interface {
private:
	const int type;
	int claim_count;
protected:
	const interface_descriptor_t *desc;
	Device *device;
	Config *config;
	std::vector<Endpoint *>altsettings;
public:
	Interface(Device *device, Config *config, const interface_descriptor_t *desc, const int &type = INTERFACE_UNKNOWN);
	virtual ~Interface();
	inline Device *get_device() { return device; };
	inline const int get_type() const { return type; };
	inline const interface_descriptor_t *get_descriptor() { return desc; };
	inline uint8_t get_interface_number() { return desc ? desc->bInterfaceNumber : (uint8_t)0; };
	int add_alt_setting(Endpoint *endpoint);
	/**
	 * 代替設定(Endpoint)を取得
	 * @param ix 代替設定のインデックス
	 */
	const Endpoint *get_alt_setting(const int &ix);
	/**
	 * 代替設定(Endpoint)を取得
	 * @param ix endpoint_address エンドポイントアドレス
	 */
	const Endpoint *get_endpoint(const uint8_t &endpoint_address);
	inline std::vector<Endpoint *> &get_alt_settings() { return altsettings; };
	int claim();
	int release(bool force = false);
	int set_interface(uint8_t alt_setting);
	int set_interface(Endpoint *endpoint);
	const bool is_iso() const;
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* INTERFACE_H_ */
