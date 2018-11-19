/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef DESCRIPTOR_H_
#define DESCRIPTOR_H_

#include "utilbase.h"
#include "json_helper.h"
// core
#include "aandusb.h"
#include "internal.h"
#include "config.h"

namespace serenegiant {
namespace usb {

#if defined(NDEBUG)
	#define LOG_WAIT(us)
#else
	#define LOG_WAIT(us) usleep(us)
#endif

#define	KEY_INDEX					"index"
#define KEY_TYPE					"type"
#define KEY_SUBTYPE					"subType"
#define KEY_WIDTH					"width"
#define KEY_HEIGHT					"height"
#define	KEY_VALUE					"value"
#define KEY_DETAIL					"detail"
#define KEY_DEFAULT                 "default"

#define DESCRIPTION					"descriptor"
#define DESC_SUBTYPE				KEY_SUBTYPE
#define	DESC_VENDERID				"venderId"
#define	DESC_PRODUCTID				"productId"
#define	DESC_SERIALNUMBER			"serialNumber"
#define DESC_MANIFUCTURE			"manifuctureName"
#define DESC_PRODUCT				"productName"
#define DESC_UVC					"uvc"
#define DESC_VIDEO_CONTROL			"videoControl"
#define DESC_INTERFACES				"interfaces"
#define DESC_BOS					"bos"

#define INTERFACE_TYPE 				KEY_TYPE
#define INTERFACE_TYPE_VIDEOSTREAM	"videoStreaming"
#define INTERFACE_TYPE_AUDIOSTREAM	"audioStreaming"
#define INTERFACE_INDEX				KEY_INDEX
#define INTERFACE_ENDPOINT_ADDR		"endpointAddress"

using namespace rapidjson;

class Interface;

// ディスクリプタを列挙するための列挙子
class descriptor_iter {
private:
	const uint8_t *config_start;	// 先頭位置
    const uint8_t *config_end;		// 最終位置
    const uint8_t *curr_desc;		// 現在位置
protected:
public:
    descriptor_iter(const uint8_t *desc, const size_t &length) {
        this->config_start = desc;
        this->config_end = desc + length;
        this->curr_desc = desc;
    }
    descriptor_iter(const descriptor_iter &other) {
        this->config_start = other.config_start;
        this->config_end = other.config_end;
        this->curr_desc = other.curr_desc;
    }
    descriptor_iter(const descriptor_iter &other, const size_t &length) {
        this->config_start = other.curr_desc;
        this->config_end = other.curr_desc + length;
        if (UNLIKELY(this->config_end > other.config_end))
        	this->config_end = other.config_end;
        this->curr_desc = other.curr_desc;
    }
    /** 次のディスクリプタの先頭へのポインタを取得して現在位置(次のディスクリプタの先頭位置)を1つ進める */
    const descriptor_header_t *next() {
    	descriptor_header_t *next;
        if (UNLIKELY((curr_desc < config_start) || (curr_desc >= config_end)))
            return NULL;
        next = (descriptor_header_t *)curr_desc;
        if (UNLIKELY(!next->bLength)) {
        	LOGW("zero length descriptor:start=%p,end=%p,current=%p", config_start, config_end, curr_desc);
            return NULL;
        }
        curr_desc += next->bLength;
        return next;
    }
    /** 指定したバイト数分前に戻す */
    void back(const size_t length) {
       	curr_desc -= length;
    }
    /** 指定したバイト数分をスキップする */
    void skip(const size_t length) {
    	curr_desc += length;
    }
    /** 次のディスクリプタの先頭へのポインタを取得する。現在位置は変更しない */
    inline const uint8_t *get_current() { return curr_desc; };
    /** この列挙師の先頭から現在位置(次のディスクリプタの先頭)までのバイト数を取得する */
    inline const size_t current_length() { return curr_desc - config_start; }
    inline void dump() {
    	LOGV("start=%p,end=%p,current=%p", config_start, config_end, curr_desc);
    }
};

using namespace rapidjson;

class Device;
class Config;
class Endpoint;

uint64_t nbytes(const uint8_t *ptr, int size);

/**
*
*/
class Descriptor {
private:
protected:
	std::vector<const device_descriptor_t *> devices;
//	std::vector<const config_descriptor_t *> configs;
	std::vector<Config *> configs;
	Config *currentConfig;
	std::vector<Interface *> *interfaces;
	std::vector<const interface_assoc_descriptor_t *> *associations;
	std::vector<const endpoint_descriptor_t *> *endpoints;
	std::vector<const dev_cap_header_t *> *capabilities;
	std::vector<const descriptor_header_t *> *others;
	bos_descriptor_t *bosDescriptor;
	Device *device;
	const char *get_str(int id, char *str_buf, const size_t &buff_len);
	// toCstrの孫請け
	virtual void write_device_descriptor(Writer<StringBuffer> &writer, const device_descriptor_t *desc);
	virtual void write_config_descriptor(Writer<StringBuffer> &writer, const Config *config);
	virtual void write_interface_association_descriptor(Writer<StringBuffer> &writer, const interface_assoc_descriptor_t *desc);
	virtual void write_interface_descriptor(Writer<StringBuffer> &writer, Interface *interface);
	virtual void write_endpoint_descriptor(Writer<StringBuffer> &writer, const endpoint_descriptor_t *desc);
	virtual void write_bos_descriptor(Writer<StringBuffer> &writer, const bos_descriptor_t *desc);
	virtual void write_capability_descriptor(Writer<StringBuffer> &writer, const dev_cap_header_t *desc);
	virtual void write_wireless_cap_descriptor(Writer<StringBuffer> &writer, const wireless_cap_descriptor_t *desc);
	virtual void write_ext_cap_descriptor(Writer<StringBuffer> &writer, const ext_cap_descriptor_t *desc);
	virtual void write_ss_cap_descriptor(Writer<StringBuffer> &writer, const ss_cap_descriptor_t *desc);
	virtual void write_ss_container_id_descriptor(Writer<StringBuffer> &writer, const ss_container_id_descriptor_t *desc);
	virtual void write_descriptor(Writer<StringBuffer> &writer, const descriptor_header_t *desc);
	// toCstrの下請け
	virtual void write_devices(Writer<StringBuffer> &writer);
	virtual void write_configs(Writer<StringBuffer> &writer);
	virtual void write_interface_association(Writer<StringBuffer> &writer);
	virtual void write_interfaces(Writer<StringBuffer> &writer);
	virtual void write_endpoints(Writer<StringBuffer> &writer);
	virtual void write_capabilities(Writer<StringBuffer> &writer);
	virtual void write_others(Writer<StringBuffer> &writer);
	//
	virtual void parse(const uint8_t *_desc, const size_t &length);
	virtual char *toCstr(const uint8_t *_desc, const size_t &length);
	virtual void parse_descriptor(descriptor_iter &iter, const descriptor_header_t *desc);
	// general
	virtual void parse_device_descriptor(descriptor_iter &iter, const device_descriptor_t *desc);
	virtual void parse_config_descriptor(descriptor_iter &iter, const config_descriptor_t *desc);
	virtual Config *create_config(const config_descriptor_t *desc);
	virtual void parse_string_descriptor(descriptor_iter &iter, const string_descriptor_t *desc);
	virtual void parse_interface_association_descriptor(descriptor_iter &iter, const interface_assoc_descriptor_t *desc);
	virtual Interface *parse_interface_descriptor(descriptor_iter &iter, const interface_descriptor_t *desc);
	virtual Endpoint *parse_endpoint_descriptor(descriptor_iter &iter,
		const interface_descriptor_t *intf_desc,
		const endpoint_descriptor_t *desc);
	virtual void parse_bos_descriptor(descriptor_iter &iter, const bos_descriptor_t *desc);
	virtual const dev_cap_header_t *parse_device_capability(descriptor_iter &iter, const dev_cap_header_t *desc);
	virtual const ss_ep_comp_descriptor_t *parse_ss_endpoint_comp_descriptor(descriptor_iter &iter, const ss_ep_comp_descriptor_t *desc);
	virtual const ssp_isoc_ep_comp_descriptor_t *parse_ssp_isoc_ep_comp_descriptor(descriptor_iter &iter, const ssp_isoc_ep_comp_descriptor_t *desc);
	// class specific
	virtual void parse_cs_device_descriptor(descriptor_iter &iter, const descriptor_header_t *desc);
	virtual void parse_cs_config_descriptor(descriptor_iter &iter, const descriptor_header_t *desc);
	virtual void parse_cs_string_descriptor(descriptor_iter &iter, const string_descriptor_t *desc);
	virtual void parse_cs_interface_descriptor(descriptor_iter &iter, const descriptor_header_t *desc);
	virtual void parse_cs_endpoint_descriptor(descriptor_iter &iter, const descriptor_header_t *desc);

public:
	Descriptor();
	virtual ~Descriptor();
	virtual void parse(Device *_device);
	virtual char *to_cstr();
	inline const std::vector<const device_descriptor_t *>&getDevices() { return devices; };
	inline const std::vector<Config *> get_configs() { return configs; };
	virtual void set_current_config(Config *current);
	Config *get_current_config();
	inline const std::vector<Interface *> *get_interfaces() { get_current_config(); return interfaces; };
	Interface *get_interface(const uint8_t &interface_number, const uint8_t &bInterfaceClass, const uint8_t &bInterfaceSubClass);
	const std::vector<Interface *> get_interfaces(const uint8_t &bInterfaceClass, const uint8_t &bInterfaceSubClass);
	inline const std::vector<const interface_assoc_descriptor_t *> *get_associations() { get_current_config(); return associations; };
	inline const std::vector<const endpoint_descriptor_t *> *get_endPoints() { get_current_config(); return endpoints; };
	inline const std::vector<const descriptor_header_t *> *get_others() { get_current_config(); return others; };
	inline const bool is_usb3() const { return devices.size() > 0 ? letoh16(devices[0]->bcdUSB) >= 0x300 : false; };
	static void dump_nbytes(const char *prefix, const void *ptr, int size);
	static void dump_guid(const char *prefix, const uint8_t *guid);
	virtual void dump_descriptor(const char *title, const descriptor_header_t *desc);
	virtual void dump_device_descriptor(const device_descriptor_t *desc);
	virtual void dump_config_descriptor(const config_descriptor_t *desc);
	virtual void dump_interface_association_descriptor(const interface_assoc_descriptor_t *desc);
	virtual void dump_interface_descriptor(const interface_descriptor_t *desc);
	virtual void dump_endpoint_descriptor(const endpoint_descriptor_t *desc);
	virtual void dump_bos_descriptor(const bos_descriptor_t *desc);
	virtual void dump_ss_endpoint_comp_descriptor(const ss_ep_comp_descriptor_t *desc);
	virtual void dump_ssp_isoc_ep_comp_descriptor(const ssp_isoc_ep_comp_descriptor_t *desc);
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* DESCRIPTOR_H_ */
