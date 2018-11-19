/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UPC_FRAME_H_
#define UPC_FRAME_H_

// core
#include "core/internal.h"
// upc
#include "androupc.h"

#define USE_MALLOC 0

upc_raw_frame_t getRawFrameFormat(const upc_frame_format_t &format);
upc_frame_format_t getFrameFormat(const upc_raw_frame_t &raw_frame);

// このクラスでの変換処理は今の実装では全てCPU上で実行されるのであまり早くない
// 可能な限りGPUで処理するようにすべき
// RGB888やBGR888は1ピクセル3バイトで2/4バイト境界にアライメント出来きず遅いので
// 使うべきではない
/**
*
*/
class UPCVideoFrame {
private:
protected:
	upc_frame_format_t frame_format;
	uint32_t width;
	uint32_t height;
	uint32_t step;
	uint32_t pixelBytes;
	uint32_t actual_bytes;
	nsecs_t _presentationtimeUs;
	uint32_t sequence;
#if USE_MALLOC
	uint8_t *frame;
	uint32_t mSize;
#else
	std::vector<uint8_t> frame;
#endif
public:
	UPCVideoFrame();						// デフォルトコンストラクタ
	UPCVideoFrame(const uint32_t &bytes);	// サイズ指定付きコンストラクタ
	UPCVideoFrame(const uint32_t &width, const uint32_t &height, const upc_frame_format_t &format); // サイズ指定付きコンストラクタ
	UPCVideoFrame(UPCVideoFrame &other);			// コピーコンストラクタ
	virtual ~UPCVideoFrame();				// デストラクタ
	inline uint8_t &operator[](uint32_t ix) { return frame[ix]; }
	UPCVideoFrame &operator=(const UPCVideoFrame &other);	// 代入演算子(ディープコピー)

	int copyTo(UPCVideoFrame &dest);

	// バッファサイズを変更する
	uint32_t resize(const uint32_t &bytes);
	int resize(const uint32_t &width, const uint32_t &height, const uint32_t &pixel_bytes);
	int resize(UPCVideoFrame &other, const upc_frame_format_t &format);
	/** バッファを開放して保持しているデータサイズもクリアする */
	void recycle();
	/** 保持しているデータサイズをクリアする, 実際のバッファ自体はそのまま */
	inline void clear() { actual_bytes = 0; };

	inline const upc_frame_format_t &getFormat() const { return frame_format; };
	void setFormat(const uint32_t &width, const uint32_t &height, const upc_frame_format_t &format);
	void setFormat(const upc_frame_format_t &format);
	void setFormat(const upc_raw_frame_t &raw_frame) { setFormat(getFrameFormat(raw_frame)); };
	inline const uint32_t &getActualBytes() { return actual_bytes; };
#if USE_MALLOC
	inline const uint32_t size() { return mSize; };
#else
	inline const uint32_t size() { return frame.size(); };
#endif
	inline uint8_t *getFrame() { return &frame[0]; };
	inline const uint32_t &getWidth() const { return width; };
	inline const uint32_t &getHeight() const { return height; };
	inline const uint32_t &getStep() const { return step; };

	inline const nsecs_t &updatePresentationtimeUs(const uint32_t &_sequence) {
		sequence = _sequence;
		_presentationtimeUs = systemTime() / 1000;
		return _presentationtimeUs;
	};
	inline const nsecs_t &presentationtimeUs() const { return _presentationtimeUs; };
};

#endif /* UPC_FRAME_H_ */
