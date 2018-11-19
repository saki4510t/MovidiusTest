/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_FORMAT_H_
#define UPC_FORMAT_H_

// upc
#include "androupc.h"
#include "upc_descriptor.h"

class Interface;

/**
*
*/
class UPCFormat {
private:
	Interface *interface;
	UPCDescriptor *upc_descriptor;
	std::vector<const frame_descriptor_t *> frame_descs;
	void init(upc_raw_format_t raw_format);
protected:
	format_descriptor_t format_desc;
	int find_stream_uncompressed(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const uint32_t &width, const uint32_t &height, const uint32_t &min_fps, const uint32_t &max_fps);
public:
	UPCFormat(Interface *interface, const format_uncompressed_t *format_desc);
	UPCFormat(Interface *interface, const format_mjpeg_t *format_desc);
	virtual ~UPCFormat();
	inline const int getFormatIndex() const { return format_desc.format_index; };
	inline const int getDefaultFrameIndex() const { return format_desc.common->bDefaultFrameIndex; };
	inline const upc_raw_frame_t getFrameType() const { return format_desc.frame_type; };
	inline Interface *getInterface() { return interface; };
	void addFrame(const upc_descriptor_header_t *frame_desc);
	inline const format_descriptor_t &getFormatDescriptor() { return format_desc; };
	inline std::vector<const frame_descriptor_t *> &getFrames() { return frame_descs; };
	const frame_descriptor_t *getFrameDescriptor(const int format_index, const int frame_index);
	int find_stream(streaming_control_t &result,
		const streaming_control_t &cur, const streaming_control_t &min, const streaming_control_t &max,
		const upc_raw_frame &frame_type, const uint32_t &width, const uint32_t &height, const uint32_t &min_fps, const uint32_t &max_fps);
};

#endif /* UPC_FORMAT_H_ */
