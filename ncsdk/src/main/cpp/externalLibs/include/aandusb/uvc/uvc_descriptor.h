/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_DESCRIPTOR_H_
#define UVC_DESCRIPTOR_H_

//#define TEST_PARSE
//#define LOCAL_DUMP

#include "utilbase.h"
// core
#include "core/descriptor.h"
// uvc
#include "aanduvc.h"
#include "UvcH264.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class Format;
class ConfigH264;
class VideoControlInterface;
class VideoInputStreamInterface;

/**
*
*/
class UVCDescriptor : public Descriptor {
friend class UVCStream;
private:
	bool is_uvc;
	bool haveTISCamera;
	bool parse_vc;				// Video Control Descriptor解析中
	VideoControlInterface *vc_interface;
	bool parse_vs_input;		// Video Stream Descriptor(input)解析中
	bool parse_vs_output;		// Video Stream Descriptor(output)解析中
	bool parse_vs_interface;	// Video Stream Interface解析中
	Interface *vs_interface;	// これは作業用なので解析中以外は使わないこと

	std::vector<const input_terminal_descriptor_t *> *input_terminals;
	std::vector<const output_terminal_descriptor_t *> *output_terminals;
	std::vector<const selector_unit_descriptor_t *> *selector_units;
	std::vector<const processing_unit_descriptor_t *> *processing_units;
	std::vector<const extension_unit_descriptor_t *> *extension_units;
	std::vector<const encoding_unit_descriptor_t *> *encoding_units;
	std::vector<const still_image_frame_descriptor_t *> *still_frames;	// 多分１つだけやねんけど
	std::vector<Format *> *formats;	// VS format descriptorリスト(それぞれのFormat下にVS frame descriptorリストが存在)
	std::vector<ConfigH264 *> *h264_configs;

	virtual Config *create_config(const config_descriptor_t *desc);
	// parse_cs_interface_descriptor => parse_video_control
	void parse_video_control(descriptor_iter &_iter, const uvc_descriptor_header_t *desc);
	// parse_video_controlの下請け
	void parse_vc_header(descriptor_iter &_iter, const uvc_header_descriptor_t *desc);
	void parse_vc_input_terminal(descriptor_iter &_iter, const input_terminal_descriptor_t *desc);
	void parse_vc_output_terminal(descriptor_iter &_iter, const output_terminal_descriptor_t *desc);
	void parse_vc_selector_unit(descriptor_iter &_iter, const selector_unit_descriptor_t *desc);
	void parse_vc_processing_unit(descriptor_iter &_iter, const processing_unit_descriptor_t *desc);
	void parse_vc_extension_unit(descriptor_iter &_iter, const extension_unit_descriptor_t *desc);
	void parse_vc_encoding_unit(descriptor_iter &_iter, const encoding_unit_descriptor_t *desc);
	void dump_vc_header(const uvc_header_descriptor_t *desc);
	void dump_vc_input_terminal(const input_terminal_descriptor_t *desc);
	void dump_vc_output_terminal(const output_terminal_descriptor_t *desc);
	void dump_vc_selector_unit(const selector_unit_descriptor_t *desc);
	void dump_vc_processing_unit(const processing_unit_descriptor_t *desc);
	void dump_vc_extension_unit(const extension_unit_descriptor_t *desc);
	void dump_vc_encoding_unit(const encoding_unit_descriptor_t *desc);
	void dump_uvcx_video_config_probe_commit(const uvcx_video_config_probe_commit_t *config);
	// parse_vc_input_terminalの下請け
	void parse_vc_camera_terminal(descriptor_iter &_iter, const camera_terminal_descriptor_t *desc);
	// parse_cs_interface_descriptor => parse_video_stream
	void parse_video_stream(descriptor_iter &_iter, const uvc_descriptor_header_t *_desc);
	// parse_video_streamの下請け
	void parse_vs_input_header_descriptor(descriptor_iter &_iter, const input_header_descriptor_t *desc);
	void parse_vs_output_header_descriptor(descriptor_iter &_iter, const output_header_descriptor_t *desc);
	void parse_vs_still_image_frame(descriptor_iter &_iter, const uvc_descriptor_header_t *desc);
	void parse_vs_format_uncompressed(descriptor_iter &_iter, const format_uncompressed_t *desc);
	void parse_vs_frame_uncompressed(descriptor_iter &_iter, const frame_uncompressed_t *desc);
	void parse_vs_format_mjpeg(descriptor_iter &_iter, const format_mjpeg_t *desc);
	void parse_vs_frame_mjpeg(descriptor_iter &_iter, const frame_mjpeg_t *desc);
	void parse_vs_format_mpeg2ts(descriptor_iter &_iter, const uvc_descriptor_header_t *desc);	// XXX
	void parse_vs_format_dv(descriptor_iter &_iter, const uvc_descriptor_header_t *desc);	// XXX
	void parse_vs_color_matching_descriptor(descriptor_iter &_iter, const color_matching_descriptor_t *desc);	// XXX
	void parse_vs_format_frame(descriptor_iter &_iter, const format_frame_based_t * desc);
	void parse_vs_frame_frame(descriptor_iter &_iter, const frame_frame_based_t * desc);
	void parse_vs_format_stream(descriptor_iter &_iter, const uvc_descriptor_header_t * desc);	// XXX
	void parse_vs_format_h264(descriptor_iter &_iter, const format_h264_t *desc);
	void parse_vs_frame_h264(descriptor_iter &_iter, const frame_h264_t *desc);
	void parse_vs_format_h264_simulcast(descriptor_iter &_iter, const format_h264_t *desc);
	void parse_vs_format_vp8(descriptor_iter &_iter, const format_vp8_t *desc);
	void parse_vs_frame_vp8(descriptor_iter &_iter, const frame_vp8_t *desc);
	void parse_vs_format_vp8_simulcast(descriptor_iter &_iter, const format_vp8_t *desc);
	void dump_vs_input_header_descriptor(const input_header_descriptor_t *desc);
	void dump_vs_output_header_descriptor(const output_header_descriptor_t *desc);
	void dump_vs_still_image_frame(const still_image_frame_descriptor_t *desc);
	void dump_vs_format_uncompressed(const format_uncompressed_t *desc);
	void dump_vs_frame_uncompressed(const frame_uncompressed_t *desc);
	void dump_vs_format_mjpeg(const format_mjpeg_t *desc);
	void dump_vs_frame_mjpeg(const frame_mjpeg_t *desc);
	void dump_vs_format_mpeg2ts(const uvc_descriptor_header_t *desc);	// XXX
	void dump_vs_format_dv(const uvc_descriptor_header_t *desc);	// XXX
	void dump_vs_color_matching_descriptor(const color_matching_descriptor_t *desc);
	void dump_vs_format_frame(const format_frame_based_t * desc);
	void dump_vs_frame_frame(const frame_frame_based_t * desc);
	void dump_vs_format_stream(const uvc_descriptor_header_t * desc); // XXX
	void dump_vs_format_h264(const format_h264_t *desc);
	void dump_vs_frame_h264(const frame_h264_t *desc);
	void dump_vs_format_vp8(const format_vp8_t *desc);
	void dump_vs_frame_vp8(const frame_vp8_t *desc);
	//
	void dump_vs_streaming_control(const streaming_control_t *ctrl);
	void dump_video_still_control(const video_still_control_t *ctrl);
	//
	void write_vc_header(Writer<StringBuffer> &writer, const uvc_header_descriptor_t *desc);
	void write_vc_input_terminal(Writer<StringBuffer> &writer, const input_terminal_descriptor_t *desc);
	void write_vc_output_terminal(Writer<StringBuffer> &writer, const output_terminal_descriptor_t *desc);
	void write_vc_selector_unit(Writer<StringBuffer> &writer, const selector_unit_descriptor_t *desc);
	void write_vc_processing_unit(Writer<StringBuffer> &writer, const processing_unit_descriptor_t *desc);
	void write_vc_extension_unit(Writer<StringBuffer> &writer, const extension_unit_descriptor_t *desc);
//
	void write_vs_input_header_descriptor(Writer<StringBuffer> &writer, const input_header_descriptor_t *desc);
	void write_vs_output_header_descriptor(Writer<StringBuffer> &writer, const output_header_descriptor_t *desc);
	void write_vs_still_image_frame(Writer<StringBuffer> &writer, const still_image_frame_descriptor_t *desc);
	void write_vs_format(Writer<StringBuffer> &writer, Format *format);
	void write_vs_format_uncompressed(Writer<StringBuffer> &writer, const format_uncompressed_t *desc);
	void write_vs_frame_uncompressed(Writer<StringBuffer> &writer, const frame_uncompressed_t *desc);
	void write_vs_format_mjpeg(Writer<StringBuffer> &writer, const format_mjpeg_t *desc);
	void write_vs_frame_mjpeg(Writer<StringBuffer> &writer, const frame_mjpeg_t *desc);
	void write_vs_format_mpeg2ts(Writer<StringBuffer> &writer, const uvc_descriptor_header_t *desc);	// XXX
	void write_vs_format_dv(Writer<StringBuffer> &writer, const uvc_descriptor_header_t *desc);	// XXX
	void write_vs_color_matching_descriptor(Writer<StringBuffer> &writer, const color_matching_descriptor_t *desc);
	void write_vs_format_frame(Writer<StringBuffer> &writer, const format_frame_based_t *desc);	// XXX
	void write_vs_frame_frame(Writer<StringBuffer> &writer, const frame_frame_based_t *desc);	// XXX
	void write_vs_format_stream(Writer<StringBuffer> &writer, const uvc_descriptor_header_t *desc); // XXX
	void write_vs_format_h264(Writer<StringBuffer> &writer, const format_h264_t *desc);
	void write_vs_frame_h264(Writer<StringBuffer> &writer, const frame_h264_t *desc);
	void write_vs_format_vp8(Writer<StringBuffer> &writer, const format_vp8_t *desc);
	void write_vs_frame_vp8(Writer<StringBuffer> &writer, const frame_vp8_t *desc);
	//
	void write_vs_streaming_control(Writer<StringBuffer> &writer, const streaming_control_t *ctrl);
	void write_fps(Writer<StringBuffer> &writer,
		std::vector<const frame_descriptor_t *> &frames,
		const int &max_width = -1, const int &max_height = -1);
protected:
	// UVCDescriptorではpublicにする
//	virtual void parse(const uint8_t *_desc, const size_t &length);
	// toCstrの孫請け
//	virtual void write_device_descriptor(Writer<StringBuffer> &writer, const device_descriptor_t *desc);
//	virtual void write_config_descriptor(Writer<StringBuffer> &writer, const config_descriptor_t *desc);
//	virtual void write_interface_association_descriptor(Writer<StringBuffer> &writer, const interface_assoc_descriptor_t *desc);
	virtual void write_interface_descriptor(Writer<StringBuffer> &writer, Interface *interface);
	virtual void write_endpoint_descriptor(Writer<StringBuffer> &writer, const endpoint_descriptor_t *desc);
//	virtual void write_descriptor(Writer<StringBuffer> &writer, const descriptor_header_t *desc);
	// toCstrの下請け
//	virtual void write_devices(Writer<StringBuffer> &writer);
//	virtual void write_configs(Writer<StringBuffer> &writer);
//	virtual void write_interface_association(Writer<StringBuffer> &writer);
	virtual void write_interfaces(Writer<StringBuffer> &writer);
	virtual void write_endpoints(Writer<StringBuffer> &writer);
//	virtual void write_others(Writer<StringBuffer> &writer);
	// general
	virtual void parse_device_descriptor(descriptor_iter &_iter, const device_descriptor_t *_desc);
//	virtual void parse_config_descriptor(descriptor_iter &_iter, const config_descriptor_t *_desc);
//	virtual void parse_string_descriptor(descriptor_iter &_iter, const string_descriptor_t *_desc);
//	virtual void parse_interface_association_descriptor(descriptor_iter &_iter, const interface_assoc_descriptor_t *_desc);
	virtual Interface *parse_interface_descriptor(descriptor_iter &_iter, const interface_descriptor_t *_desc);
	virtual Endpoint *parse_endpoint_descriptor(descriptor_iter &_iter,
		const interface_descriptor_t *intf_desc,
		const endpoint_descriptor_t *desc);
	// class specific
//	virtual void parse_cs_device_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_config_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_string_descriptor(descriptor_iter &_iter, const string_descriptor_t *_desc);
	virtual void parse_cs_interface_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
	virtual void parse_cs_endpoint_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);

public:
	UVCDescriptor();
	virtual ~UVCDescriptor();

	virtual void parse(Device *_device);
	// UVCDescriptorではpublicにする
	virtual void parse(const uint8_t *_desc, const size_t &length);
	inline const bool isUVC() const { return is_uvc; };
	inline const bool has_h264() const { return h264_configs ? h264_configs->size() > 0 : false; };
	const bool has_uvc15_config() const;

	inline std::vector<Format *> *get_formats() { get_current_config(); return formats; };
	inline std::vector<const still_image_frame_descriptor_t *> * get_still_frames() { get_current_config(); return still_frames;};
	inline std::vector<const input_terminal_descriptor_t *> *get_input_terminals() { get_current_config(); return input_terminals; };
	inline std::vector<const output_terminal_descriptor_t *> *get_output_terminals() { get_current_config(); return output_terminals; };
	inline std::vector<const selector_unit_descriptor_t *> *get_selector_units() { get_current_config(); return selector_units; };
	inline std::vector<const processing_unit_descriptor_t *> *get_processing_units() { get_current_config(); return processing_units; };
	inline std::vector<const extension_unit_descriptor_t *> *get_extension_units() { get_current_config(); return extension_units; };
	inline std::vector<ConfigH264 *> *get_h264_configs() { get_current_config(); return h264_configs; };

	virtual void set_current_config(Config *current);
	VideoControlInterface *get_ctrl_interface();
	const std::vector<VideoInputStreamInterface *> get_stream_interfaces();

	const raw_frame_t get_raw_frame_type(const uint8_t &stream_interface_number, const uint8_t &format_index);

	const format_descriptor_t *get_format_descriptor(const uint8_t &stream_interface_number, const uint8_t &format_index);
	inline const format_descriptor_t *get_format_descriptor(
		const uint8_t &stream_interface_number, const streaming_control_t &ctrl) {

		return get_format_descriptor(stream_interface_number, ctrl.bFormatIndex);
	};

	const frame_descriptor_t *get_frame_descriptor(const uint8_t format_index, const uint8_t frame_index);
	inline const frame_descriptor_t *get_frame_descriptor(const streaming_control_t &ctrl) {
		return get_frame_descriptor(ctrl.bFormatIndex, ctrl.bFrameIndex);
	};

	const still_image_frame_descriptor_t *get_still_frame_descriptor();
	int get_still_frame_size(const int size_index, uint32_t &width, uint32_t &height);

	char *get_supported_size();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_DESCRIPTOR_H_ */
