/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_FORMAT_H264_H_
#define UVC_FORMAT_H264_H_

#include "utilbase.h"
// uvc
#include "aanduvc.h"
#include "UvcH264.h"

namespace serenegiant {
namespace usb {
namespace uvc {

/**
*　h264ストリームが他の映像ストリームと多重化されている時のコンフィギュレーション保持用のクラス(as外部ストリーム)
 * 単独のインターフェースとして実装されている場合は通常のフォーマットディスクリプタとして検出されるはず
*/
class ConfigH264 {
private:
	const extension_unit_descriptor_t *unit_desc;
	std::vector<const uvcx_video_config_probe_commit_t *> configs;
//	void init(raw_format_t raw_format);
protected:
public:
	ConfigH264(const extension_unit_descriptor_t *unit_desc);
	virtual ~ConfigH264();
	void add_config(const uvcx_video_config_probe_commit_t &config);

	inline const extension_unit_descriptor_t *get_descriptor() { return unit_desc; };
	inline const uint8_t get_unit_id() { return unit_desc->bUnitID; };
	inline std::vector<const uvcx_video_config_probe_commit_t *> &get_configs() { return configs; };
	const uvcx_video_config_probe_commit_t *get_config(const int config_index);
	int find_stream(uvcx_video_config_probe_commit_t &result,
		const uvcx_video_config_probe_commit_t &cur,
		const uint32_t &width, const uint32_t &height,
		const float &min_fps, const float &max_fps,
		const float &bandwidth_factor = 0.0f);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_FORMAT_H264_H_ */
