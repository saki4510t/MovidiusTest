/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UVC_FRAME_POOL_H
#define USBWEBCAMERAPROJ_UVC_FRAME_POOL_H

#include <vector>

// core
#include "core/internal.h"
#include "core/frame_pool.h"
// uvc
#include "uvc_frame.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uvc {


class UVCFramePool : virtual public FramePool<VideoFrame> {
private:
	const dct_mode_t _dct_mode;
protected:
	UVCFramePool(const uint32_t &max_frame_num = DEFAULT_MAX_FRAME_NUM,
		const uint32_t &init_frame_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &_default_frame_sz = DEFAULT_FRAME_SZ,
		const bool &create_if_empty = false, const bool &block_if_empty = false,
		const dct_mode_t &dct_mode = DEFAULT_DCT_MODE)
	: FramePool<VideoFrame>(max_frame_num, init_frame_num,
		_default_frame_sz, create_if_empty, block_if_empty),
	  _dct_mode(dct_mode) {
	}

	virtual VideoFrame *create_frame(const size_t &data_bytes) {
		return new VideoFrame(data_bytes, _dct_mode);
	}
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UVC_FRAME_POOL_H
