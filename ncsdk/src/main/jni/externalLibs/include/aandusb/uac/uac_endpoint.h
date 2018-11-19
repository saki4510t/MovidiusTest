/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_ENDPOINT_H_
#define UAC_ENDPOINT_H_

#include "utilbase.h"
// core
#include "core/endpoint.h"
// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {
namespace uac {

/**
*
*/
class IsochronousAudioDataEndpoint: public Endpoint {
private:
	const uac1_as_header_descriptor_t *header_desc;
	const uac_format_descriptor_t *format_desc;
	const uac_iso_endpoint_descriptor_t *isochronous_audio_data;
public:
	IsochronousAudioDataEndpoint(const interface_descriptor_t *_interface,
		const endpoint_descriptor_t *_endpoint,
		const uac1_as_header_descriptor_t *_header_desc,
		const uac_format_descriptor_t *_format_desc,
		const uac_iso_endpoint_descriptor_t *_isochronous_audio_data);
	virtual ~IsochronousAudioDataEndpoint();
	inline const uac1_as_header_descriptor_t *get_header() const { return header_desc; };
	inline const uac_format_descriptor_t *get_format() const { return format_desc; };
	inline const uac_iso_endpoint_descriptor_t *get_isochronous_audio_data() const { return isochronous_audio_data; };
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_ENDPOINT_H_ */
