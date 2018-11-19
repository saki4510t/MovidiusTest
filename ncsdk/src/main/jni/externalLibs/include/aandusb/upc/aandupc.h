/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef ANDROUPC_H_
#define ANDROUPC_H_

// core
#include "aandusb.h"

// -100以下
enum upc_errors {
	UPC_ERROR_DUPLICATE_ENDPOINT = -100,
};

typedef enum upc_raw_format {
	UPC_FORMAT_UNCOMPRESSED,
	UPC_FORMAT_BAYER,
} upc_raw_format_t;

// カメラからの生フレームフォーマット
// 下1バイトはUPC_VS_FRAME_XXと同じ
typedef enum upc_raw_frame {
	UPC_RAW_FRAME_UNKNOWN = 0,
	UPC_RAW_FRAME_UNCOMPRESSED = 1,
	UPC_RAW_FRAME_BAYER = 1,
} upc_raw_frame_t;

typedef enum upc_frame_format {
	UPC_FRAME_FORMAT_UNKNOWN = 0,
	UPC_FRAME_FORMAT_YUYV,			// 1
	UPC_FRAME_FORMAT_UYVY,			// 2
	UPC_FRAME_FORMAT_GRAY8,			// 3
	UPC_FRAME_FORMAT_BY8,			// 4
	UPC_FRAME_FORMAT_RGB565,		// 5
	UPC_FRAME_FORMAT_RGB,			// 6
	UPC_FRAME_FORMAT_BGR,			// 7
	UPC_FRAME_FORMAT_RGBX,			// 8
} upc_frame_format_t;

typedef struct _uvc_streaming_control {
	uint32_t dwFrameInterval;
	uint16_t wKeyFrameRate;
	uint16_t wPFrameRate;
	uint16_t wCompQuality;
	uint16_t wCompWindowSize;
	uint32_t dwMaxVideoFrameSize;
	uint32_t dwMaxPayloadTransferSize;
} __attribute__((__packed__)) streaming_control_t;

typedef struct _format_descriptor {
	uint8_t guidFormat[16];
	uint8_t bBitsPerPixel;
	int format_index;
	uvc_raw_format_t format_type;
	uvc_raw_frame_t frame_type;
} format_descriptor_t;

#endif /* ANDROUPC_H_ */
