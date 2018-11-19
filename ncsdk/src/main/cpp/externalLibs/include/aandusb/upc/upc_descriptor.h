/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_DESCRIPTOR_H_
#define UPC_DESCRIPTOR_H_

//#define TEST_PARSE

#include "utilbase.h"
// core
#include "core/descriptor.h"
// upc
#include "androupc.h"

class UPCFormat;
class VideoControlInterface;
class VideoInputStreamInterface;

/**
*
*/
class UPCDescriptor : public Descriptor {
friend class UPCStream;
private:
	bool is_upc;
	VideoControlInterface *vc_interface;
	Interface *vs_interface;	// これは作業用なので解析中以外は使わないこと
	std::vector<UPCFormat *>formats;	// VS format descriptorリスト(それぞれのUPCFormat下にVS frame descriptorリストが存在)
	// parse_cs_interface_descriptor => parse_video_control
	void parse_video_control(descriptor_iter &_iter, const upc_descriptor_header_t *desc);
	// parse_cs_interface_descriptor => parse_video_stream
	void parse_video_stream(descriptor_iter &_iter, const upc_descriptor_header_t *_desc);
	//
	int setup_camera_params();
protected:
	virtual void parse(const uint8_t *_desc, const size_t length);
#ifdef TEST_PARSE
	virtual void parse(const uint8_t *_desc, const size_t length);
#endif
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
	virtual Endpoint *parse_endpoint_descriptor(descriptor_iter &_iter, const endpoint_descriptor_t *_desc);
	// class specific
//	virtual void parse_cs_device_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_config_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
//	virtual void parse_cs_string_descriptor(descriptor_iter &_iter, const string_descriptor_t *_desc);
	virtual void parse_cs_interface_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
	virtual void parse_cs_endpoint_descriptor(descriptor_iter &_iter, const descriptor_header_t *_desc);
public:
	UPCDescriptor();
	virtual ~UPCDescriptor();
	inline const bool isUPC() const { return is_upc; };
	inline std::vector<UPCFormat *> &getFormats() { return formats; };
	const upc_raw_frame_t getRawFrameType(const int format_index);
	const frame_descriptor_t *getFrameDescriptor(const int format_index, const int frame_index);
	inline const frame_descriptor_t *getFrameDescriptor(const streaming_control_t &ctrl) {
		return getFrameDescriptor(ctrl.bFormatIndex, ctrl.bFrameIndex);
	};
	inline VideoControlInterface *getCtrlInterface() { return (VideoControlInterface *)getInterface(USB_CLASS_VIDEO, UPC_SC_VIDEOCONTROL); };
	inline VideoInputStreamInterface *getStreamInterface() { return (VideoInputStreamInterface *)getInterface(USB_CLASS_VIDEO, UPC_SC_VIDEOSTREAMING); };
	char *getSupportedSize();
};

#endif /* UPC_DESCRIPTOR_H_ */
