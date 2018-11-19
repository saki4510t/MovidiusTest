/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UAC_FRAME_H_
#define UAC_FRAME_H_

// uac
#include "aanduac.h"

namespace serenegiant {
namespace usb {
namespace uac {

class AudioFrame;
typedef int (AudioFrame::*get_data_func_t)(const uint32_t &ix);
typedef int (AudioFrame::*set_data_func_t)(const uint32_t &ix, const int &value);

/**
*
*/
class AudioFrame {
private:
protected:
	uint32_t _err;
	size_t actual_bytes;
	uint32_t padding_front_bytes;
	uint32_t padding_back_bytes;
	nsecs_t _presentationtimeUs;
	uint32_t sequence;
	std::vector<uint8_t> frame;
	int channel_num;			// チャネル数, [1,2]
	int sample_bytes;			// 1チャネル1サンプルあたりのバイト数, [1-4]
	int sample_stride_bytes;	// 1サンプルあたりのバイト数=channel_num*sample_bytes
	const int (AudioFrame::*get_data_func_table[4])(const uint32_t &ix) = {
		&AudioFrame::get_data_1bytes, &AudioFrame::get_data_2bytes, &AudioFrame::get_data_3bytes, &AudioFrame::get_data_4bytes,
	};
	void (AudioFrame::*set_data_func_table[4])(const uint32_t &ix, const int &value) = {
		&AudioFrame::set_data_1bytes, &AudioFrame::set_data_2bytes, &AudioFrame::set_data_3bytes, &AudioFrame::set_data_4bytes,
	};
	/**
	 * 指定したインデックスのサンプルを1バイト整数のデータとみなして取得する
	 */
	const int get_data_1bytes(const uint32_t &ix);
	/**
	 * 指定したインデックスのサンプルを2バイト整数のデータとみなして取得する(リトルエンディアン)
	 * インデックス✕2が実際の先頭からのバイト数
	 */
	const int get_data_2bytes(const uint32_t &ix);
	/**
	 * 指定したインデックスのサンプルを3バイト整数のデータとみなして取得する(リトルエンディアン)
	 * インデックス✕3が実際の先頭からのバイト数
	 */
	const int get_data_3bytes(const uint32_t &ix);
	/**
	 * 指定したインデックスのサンプルを4バイト整数のデータとみなして取得する(リトルエンディアン)
	 * インデックス✕4が実際の先頭からのバイト数
	 */
	const int get_data_4bytes(const uint32_t &ix);
	/**
	 * 指定したインデックスに1バイト整数を書き込む
	 */
	void set_data_1bytes(const uint32_t &ix, const int &value);
	/**
	 * 指定したインデックスに2バイト整数を書き込む(リトルエンディアン)
	 * インデックス✕2が実際の先頭からのバイト数
	 */
	void set_data_2bytes(const uint32_t &ix, const int &value);
	/**
	 * 指定したインデックスに3バイト整数を書き込む(リトルエンディアン)
	 * インデックス✕3が実際の先頭からのバイト数
	 */
	void set_data_3bytes(const uint32_t &ix, const int &value);
	/**
	 * 指定したインデックスに4バイト整数を書き込む(リトルエンディアン)
	 * インデックス✕4が実際の先頭からのバイト数
	 */
	void set_data_4bytes(const uint32_t &ix, const int &value);
public:
	AudioFrame();						// デフォルトコンストラクタ
	AudioFrame(const size_t &bytes, const uint32_t &padding_front = 0, const uint32_t &padding_back = 0);
	AudioFrame(AudioFrame &other);		// コピーコンストラクタ
	virtual ~AudioFrame();				// デストラクタ
	inline uint8_t &operator[](uint32_t ix) { return frame[padding_front_bytes + ix]; }
	AudioFrame &operator=(const AudioFrame &other);	// 代入演算子(ディープコピー)
	inline const int operator()(const int &channel, const int &index) { return get_channel_data(
			channel, index); };
	// チャネル数と1サンプルあたりのバイト数を設定
	int setup(const int &channels, const int &sample_bytes);
	const int get_channel_data(const uint32_t &channel, const uint32_t &index);
	void set_channel_data(const uint32_t &channel, const uint32_t &index, const uint32_t &value);
	// バッファサイズを変更する(前後のpaddingは含まない)
	uint32_t resize(const size_t &bytes);
	/** バッファを開放して保持しているデータサイズもクリアする */
	void recycle();
	/** 保持しているデータサイズをクリアする, 実際のバッファ自体はそのまま */
	inline void clear() { actual_bytes = 0; _err = 0; };

	inline const uint32_t &err() const { return _err; };
	inline void set_err(const uint32_t &err) { _err = err; };
	inline const size_t &get_actual_bytes() { return actual_bytes; };
	inline const size_t size() { return frame.size(); };
	inline uint8_t *get_frame() { return &frame[padding_front_bytes]; };

	inline const nsecs_t &update_presentation_time_us(const uint32_t &_sequence) {
		sequence = _sequence;
		_presentationtimeUs = systemTime() / 1000;
		return _presentationtimeUs;
	};
	inline void set_presentation_time_us(const nsecs_t &presentationtimeUs) { _presentationtimeUs = presentationtimeUs; };
	inline const nsecs_t &presentation_time_us() const { return _presentationtimeUs; };
	inline const bool is_empty() { return !actual_bytes; };
};

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UAC_FRAME_H_ */
