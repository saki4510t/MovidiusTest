/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_FORMAT_H_
#define UVC_FORMAT_H_

// uvc
#include "aanduvc.h"
#include "uvc_descriptor.h"

namespace serenegiant {
namespace usb {

class Config;
class Interface;

namespace uvc {

class UVCDevice;

/**
*
*/
class Format {
private:
	Config *config;
	Interface *interface;
	UVCDescriptor *uvc_descriptor;
	std::vector<const frame_descriptor_t *> frame_descs;
	void init(raw_format_t raw_format);
protected:
	UVCDevice *device;
	format_descriptor_t format_desc;
	int find_stream_uncompressed(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor);
	int find_stream_mjpeg(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor);
	int find_stream_frame_based(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor);
	int find_stream_h264(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor);
	int find_stream_vp8(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor);
public:
	Format(UVCDescriptor *descriptor, Config *config, Interface *interface,
		const format_uncompressed_t *format_desc);
	Format(UVCDescriptor *descriptor, Config *config, Interface *interface,
		const format_mjpeg_t *format_desc);
	Format(UVCDescriptor *descriptor, Config *config, Interface *interface,
		const format_frame_based_t *format_desc);
	Format(UVCDescriptor *descriptor, Config *config, Interface *interface,
		const format_h264_t *format_desc, const bool &simulcast = false);
	Format(UVCDescriptor *descriptor, Config *config, Interface *interface,
		const format_vp8_t *format_desc, const bool &simulcast = false);
	virtual ~Format();
	inline Interface *get_interface() { return interface; };
	const int get_interface_number() const;
	inline const uint8_t get_format_index() const { return format_desc.format_index; };
	inline const uint8_t get_default_frame_index() const {
		return format_desc.common->bDefaultFrameIndex;
	};
	inline const raw_frame_t get_frame_type() const { return format_desc.frame_type; };
	void add_frame(const uvc_descriptor_header_t *frame_desc);
	inline const format_descriptor_t &get_format_descriptor() { return format_desc; };
	inline std::vector<const frame_descriptor_t *> &get_frames() { return frame_descs; };
	const frame_descriptor_t *get_frame_descriptor(
		const int format_index, const int frame_index);
	int find_stream(streaming_control_t &result,
		const streaming_control_t &cur,
		const streaming_control_t &min,
		const streaming_control_t &max,
		const raw_frame &frame_type, const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor = 0.0f);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_FORMAT_H_ */
