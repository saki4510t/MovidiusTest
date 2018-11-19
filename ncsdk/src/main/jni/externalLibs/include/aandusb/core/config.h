/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_CONFIG_H
#define USBWEBCAMERAPROJ_CONFIG_H

#include "utilbase.h"
#include "json_helper.h"
// core
#include "aandusb.h"
#include "internal.h"

namespace serenegiant {
namespace usb {

class Interface;

class Config {
private:
protected:
	const config_descriptor_t *config;
	std::vector<const interface_assoc_descriptor_t *> associations;
	std::vector<Interface *> interfaces;
	std::vector<const endpoint_descriptor_t *> endpoints;
	std::vector<const dev_cap_header_t *> capabilities;
	std::vector<const descriptor_header_t *> others;
	bos_descriptor_t *bosDescriptor;
public:
	Config(const config_descriptor_t *desc);
	virtual ~Config();
	inline void add(const interface_assoc_descriptor_t *desc) { associations.push_back(desc); };
	inline void add(Interface *desc) { interfaces.push_back(desc); };
	inline void add(const endpoint_descriptor_t *desc) { endpoints.push_back(desc); };
	inline void add(const dev_cap_header_t *desc) { capabilities.push_back(desc); };
	inline void add(const descriptor_header_t *desc) { others.push_back(desc); };
	inline void set(const bos_descriptor_t *desc) { bosDescriptor = (bos_descriptor_t *)desc; };

	inline const config_descriptor_t *get_config() const { return config; };
	inline std::vector<const interface_assoc_descriptor_t *> &get_associations() { return associations; };
	inline std::vector<Interface *> &get_interfaces() { return interfaces; };
	inline std::vector<const endpoint_descriptor_t *> &get_endpoints() { return endpoints; };
	inline std::vector<const dev_cap_header_t *> &get_capabilities() { return capabilities; };
	inline std::vector<const descriptor_header_t *> &get_others() { return others; };
	inline bos_descriptor_t *get_bos_descriptor() { return bosDescriptor; };
	Interface *get_interface(const uint8_t &interface_number, const uint8_t &bInterfaceClass, const uint8_t &bInterfaceSubClass);
	const std::vector<Interface *> get_interfaces(const uint8_t &bInterfaceClass, const uint8_t &bInterfaceSubClass);
	inline const unsigned int configuration_value() const { return config ? config->bConfigurationValue : 1; };
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_CONFIG_H
