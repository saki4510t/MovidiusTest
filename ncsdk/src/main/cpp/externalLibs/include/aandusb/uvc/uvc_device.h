/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_DEVICE_H_
#define UVC_DEVICE_H_

// core
#include "core/device.h"
// uvc
#include "aanduvc.h"
#include "UvcH264.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class UVCDescriptor;
class UVCStream;

/**
*
*/
class UVCDevice : public Device {
friend UVCDescriptor;
friend UVCStream;
protected:
	void dump_vs_streaming_control(const streaming_control_t &desc);
	void dump_video_still_control(const video_still_control_t &desc);
	void dump_uvcx_video_config_probe_commit(
			const uvcx_video_config_probe_commit_t &config);
public:
	UVCDevice(Context *_context, Descriptor *_descriptor,
		const char *dev_name, const int &fd);
	virtual ~UVCDevice();
	int query_ctrl(streaming_control_t &ctrl, const uint8_t &interface_number, const bool &probe, const req_code_t &req);
	int query_ctrl(video_still_control_t &ctrl, const uint8_t &interface_number, const bool &probe, const req_code_t &req);
	int query_config(uvcx_video_config_probe_commit_t &config, const uint8_t &uint_id, const bool &probe, const req_code_t &req);
	int still_trigger_ctrl(const uint8_t &interface_number, still_trigger_mode_t &trigger, const req_code_t &req);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_DEVICE_H_ */
