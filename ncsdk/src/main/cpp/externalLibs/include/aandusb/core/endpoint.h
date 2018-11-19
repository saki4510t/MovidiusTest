/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef ENDPOINT_H_
#define ENDPOINT_H_

// core
#include "aandusb.h"

namespace serenegiant {
namespace usb {

enum endpoint_direction {
	/** In: device-to-host */
	USB_ENDPOINT_IN = 0x80,

	/** Out: host-to-device */
	USB_ENDPOINT_OUT = 0x00
};

/**
*
*/
class Endpoint {
private:
protected:
	const interface_descriptor_t *interface;
	const endpoint_descriptor_t *endpoint;
	const ss_ep_comp_descriptor_t *ss_companion;
	const ssp_isoc_ep_comp_descriptor_t *ssp_companion;
public:
	Endpoint(const interface_descriptor_t *interface, const endpoint_descriptor_t *endpoint);
	Endpoint(Endpoint &src);	// コピーコンストラクタ
	virtual ~Endpoint();
	Endpoint &operator=(const Endpoint &src);	// 代入演算子(ディープコピー)
	inline const uint8_t get_altsetting() const { return interface ? interface->bAlternateSetting : (uint8_t)0; };
	inline const interface_descriptor_t *get_interface() const { return interface; };
	inline const int get_interface_number() const { return interface ? interface->bInterfaceNumber : (uint8_t)0; };
	inline const endpoint_descriptor_t *get_endpoint() const { return endpoint; };
	inline const uint8_t get_endpoint_address() const { return endpoint ? endpoint->bEndpointAddress : (uint8_t)0; };
	inline void set_companion(const ss_ep_comp_descriptor_t *ss_companion) { this->ss_companion = ss_companion; };
	inline const ss_ep_comp_descriptor_t *get_ss_companion() const { return ss_companion; };
	inline void set_companion(const ssp_isoc_ep_comp_descriptor_t *ssp_companion) { this->ssp_companion = ssp_companion; };
	inline const ssp_isoc_ep_comp_descriptor_t *get_ssp_companion() const { return ssp_companion; };
};

}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* ENDPOINT_H_ */
