/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UAC_FRAME_POOL_H
#define USBWEBCAMERAPROJ_UAC_FRAME_POOL_H

#include <vector>
// core
#include "core/internal.h"
#include "core/frame_pool.h"
// uac
#include "uac_frame.h"

#pragma interface

namespace serenegiant {
namespace usb {
namespace uac {

class UACFramePool : virtual public FramePool<AudioFrame> {
protected:
	UACFramePool(const uint32_t &max_frame_num = DEFAULT_MAX_FRAME_NUM, const uint32_t &init_frame_num = DEFAULT_INIT_FRAME_POOL_SZ,
		const size_t &_default_frame_sz = DEFAULT_FRAME_SZ,
		const bool &create_if_empty = false, const bool &block_if_empty = false)
	: FramePool<AudioFrame>(max_frame_num, init_frame_num, _default_frame_sz, create_if_empty, block_if_empty) {
	}
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UAC_FRAME_POOL_H
