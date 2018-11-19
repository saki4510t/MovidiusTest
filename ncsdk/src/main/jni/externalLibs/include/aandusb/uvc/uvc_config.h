/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UVC_CONFIG_H
#define USBWEBCAMERAPROJ_UVC_CONFIG_H

// core
#include "core/config.h"
// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {

class Interface;

namespace uvc {

class Format;
class ConfigH264;
class VideoControlInterface;
class VideoInputStreamInterface;

class UVCConfig : public Config {
protected:
	std::vector<const input_terminal_descriptor_t *> input_terminals;
	std::vector<const output_terminal_descriptor_t *> output_terminals;
	std::vector<const selector_unit_descriptor_t *> selector_units;
	std::vector<const processing_unit_descriptor_t *> processing_units;
	std::vector<const extension_unit_descriptor_t *> extension_units;
	std::vector<const encoding_unit_descriptor_t *> encoding_units;
	std::vector<const still_image_frame_descriptor_t *> still_frames;	// 多分１つだけやねんけど
	std::vector<Format *>formats;	// VS format descriptorリスト(それぞれのFormat下にVS frame descriptorリストが存在)
	std::vector<ConfigH264 *> h264_configs;
public:
	UVCConfig(const config_descriptor_t *desc);
	virtual ~UVCConfig();
	inline std::vector<const input_terminal_descriptor_t *> &get_input_terminals() { return input_terminals; };
	inline std::vector<const output_terminal_descriptor_t *> &get_output_terminals() { return output_terminals; };
	inline std::vector<const selector_unit_descriptor_t *> &get_selector_units() { return selector_units; };
	inline std::vector<const processing_unit_descriptor_t *> &get_processing_units() { return processing_units; };
	inline std::vector<const extension_unit_descriptor_t *> &get_extension_units() { return extension_units; };
	inline std::vector<const encoding_unit_descriptor_t *> &get_encoding_units() { return encoding_units; };
	inline std::vector<const still_image_frame_descriptor_t *> &get_still_frames() { return still_frames; };
	inline std::vector<Format *> &get_formats() { return formats; };
	inline std::vector<ConfigH264 *> &get_h264_configs() { return h264_configs; };

	VideoControlInterface *get_ctrl_interface();
	const std::vector<VideoInputStreamInterface *> get_stream_interfaces();
	const uint16_t bcd_uVC();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UVC_CONFIG_H
