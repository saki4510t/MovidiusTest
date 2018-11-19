/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_FRAME_H_
#define UVC_FRAME_H_

#include <turbojpeg.h>

// core
#include "core/internal.h"
// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {
namespace uvc {

//raw_frame_t getRawFrameFormat(const frame_format_t &format);
//frame_format_t getFrameFormat(const raw_frame_t &raw_frame);

typedef enum nal_unit_type {
	NAL_UNIT_UNSPECIFIED = 0,
	NAL_UNIT_CODEC_SLICE = 1,			// Coded slice of a non-IDR picture == PFrame for AVC
	NAL_UNIT_CODEC_SLICE_A = 2,			// Coded slice data partition A
	NAL_UNIT_CODEC_SLICE_B = 3,			// Coded slice data partition B
	NAL_UNIT_CODEC_SLICE_C = 4,			// Coded slice data partition C
	NAL_UNIT_CODEC_SLICE_IDR = 5,		// Coded slice of an IDR picture == IFrame for AVC
	NAL_UNIT_SEI = 6,					// supplemental enhancement information
	NAL_UNIT_SEQUENCE_PARAM_SET = 7,	// Sequence parameter set == SPS for AVC
	NAL_UNIT_PICTURE_PARAM_SET = 8,		// Picture parameter set == PPS for AVC
	NAL_UNIT_PICTURE_DELIMITER = 9,		// access unit delimiter (AUD)
	NAL_UNIT_END_OF_SEQUENCE = 10,		// End of sequence
	NAL_UNIT_END_OF_STREAM = 11,		// End of stream
	NAL_UNIT_FILLER = 12,				// Filler data
	NAL_UNIT_RESERVED_13 = 13,			// Sequence parameter set extension
	NAL_UNIT_RESERVED_14 = 14,			// Prefix NAL unit
	NAL_UNIT_RESERVED_15 = 15,			// Subset sequence parameter set
	NAL_UNIT_RESERVED_16 = 16,
	NAL_UNIT_RESERVED_17 = 17,
	NAL_UNIT_RESERVED_18 = 18,
	NAL_UNIT_RESERVED_19 = 19,			// Coded slice of an auxiliary coded picture without partitioning
	NAL_UNIT_RESERVED_20 = 20,			// Coded slice extension
	NAL_UNIT_RESERVED_21 = 21,			// Coded slice extension for depth view components
	NAL_UNIT_RESERVED_22 = 22,
	NAL_UNIT_RESERVED_23 = 23,
	NAL_UNIT_UNSPECIFIED_24 = 24,
	NAL_UNIT_UNSPECIFIED_25 = 25,
	NAL_UNIT_UNSPECIFIED_26 = 26,
	NAL_UNIT_UNSPECIFIED_27 = 27,
	NAL_UNIT_UNSPECIFIED_28 = 28,
	NAL_UNIT_UNSPECIFIED_29 = 29,
	NAL_UNIT_UNSPECIFIED_30 = 30,
	NAL_UNIT_UNSPECIFIED_31 = 31,
} nal_unit_type_t;

#define JFIF_MARKER_MSB 0xff
// 上位バイトは省略(0xff)
typedef enum jfif_marker {
	JFIF_MARKER_FF		= 0x00,	// 0xff00 == 0xffそのもの
	JFIF_MARKER_TEM		= 0x01,	// 算術符号用テンポラリ
//	JFIF_MARKER_RES		= 0x02,	// リザーブ
//	// この間は全部リザーブ
//	JFIF_MARKER_RES		= 0xBF,	// リザーブ
//	JFIF_MARKER_RES		= 0x4e,	// リザーブ
	JFIF_MARKER_SOC		= 0x4F,	// コードストリーム開始(JPEG2000)
	JFIF_MARKER_SIZ		= 0x51,	// サイズ定義(JPEG2000)
	JFIF_MARKER_COD		= 0x52,	// 符号化標準定義(JPEG2000)
	JFIF_MARKER_COC		= 0x53,	// 符号化個別定義(JPEG2000)
	JFIF_MARKER_TLM		= 0x55,	// タイルパート長定義(JPEG2000)
	JFIF_MARKER_PLM		= 0x57,	// パケット長標準定義(JPEG2000)
	JFIF_MARKER_PLT		= 0x58,	// パケット長個別定義(JPEG2000)
	JFIF_MARKER_QCD		= 0x5C,	// 量子化標準定義(JPEG2000)
	JFIF_MARKER_QCC		= 0x5D,	// 量子化個別定義(JPEG2000)
	JFIF_MARKER_RGN		= 0x5E,	// ROI定義(JPEG2000)
	JFIF_MARKER_POC		= 0x5F,	// プログレッション順序変更(JPEG2000)
	JFIF_MARKER_PPM		= 0x60,	// パケットヘッダー標準定義(JPEG2000)
	JFIF_MARKER_PPT		= 0x61,	// パケットヘッダー個別定義(JPEG2000)
	JFIF_MARKER_CRG		= 0x63,	// コンポーネント位相定義(JPEG2000)
	JFIF_MARKER_COM2	= 0x64,	// コメント(JPEG2000)
	JFIF_MARKER_SOT		= 0x90,	// タイルパート開始(JPEG2000)
	JFIF_MARKER_SOP		= 0x91,	// パケット開始(JPEG2000)
	JFIF_MARKER_EPH		= 0x92,	// パケットヘッダー終了(JPEG2000)
	JFIF_MARKER_SOD		= 0x93,	// データ開始(JPEG2000)
	JFIF_MARKER_SOF0	= 0xC0,	// ハフマン式コードのベースラインDCTフレーム	, フレームタイプ０開始セグメント
	JFIF_MARKER_SOF1	= 0xC1,	// ハフマン式コードの拡張シーケンシャルDCTフレーム, フレームタイプ１開始セグメント
	JFIF_MARKER_SOF2	= 0xC2,	// ハフマン式コードのプログレッシブDCTフレーム, フレームタイプ２開始セグメント
	JFIF_MARKER_SOF3	= 0xC3,	// ハフマン式コードの可逆(Spatial DPCM)フレーム	, フレームタイプ３開始セグメント
	JFIF_MARKER_DHT		= 0xC4,	// ハフマン法テーブル定義セグメント
	JFIF_MARKER_SOF5	= 0xC5,	// ハフマン式コードの差分拡張シーケンシャルフレーム, フレームタイプ５開始セグメント
	JFIF_MARKER_SOF6	= 0xC6,	// ハフマン式コードの差分プログレッシブフレーム, フレームタイプ６開始セグメント
	JFIF_MARKER_SOF7	= 0xC7,	// ハフマン式コードの差分可逆(Spatial DPCM)フレーム, フレームタイプ７開始セグメント
	JFIF_MARKER_JPG		= 0xC8,	// 拡張のための予備
	JFIF_MARKER_SOF9	= 0xC9,	// 算術式コードの拡張シーケンシャルDCTフレーム, フレームタイプ９開始セグメント
	JFIF_MARKER_SOF10	= 0xCA,	// 算術式コードのプログレッシブDCTフレーム, フレームタイプ１０開始セグメント
	JFIF_MARKER_SOF11	= 0xCB,	// 算術式コードの可逆(Spatial DPCM)フレーム, フレームタイプ１１開始セグメント
	JFIF_MARKER_DAC		= 0xCC,	// 算術式圧縮テーブルを定義する
	JFIF_MARKER_SOF13	= 0xCD,	// 算術式コードの差分拡張シーケンシャルDCTフレーム, フレームタイプ１３開始セグメント
	JFIF_MARKER_SOF14	= 0xCE,	// 算術式コードの差分プログレッシブDCTフレーム, フレームタイプ１４開始セグメント
	JFIF_MARKER_SOF15	= 0xCF,	// 算術式コードの差分の可逆(Spatial DPCM)フレーム, フレームタイプ１５開始セグメント
	JFIF_MARKER_RST0	= 0xd0,	// リスタートマーカー0, data 無し
	JFIF_MARKER_RST1	= 0xd1,	// リスタートマーカー1, data 無し
	JFIF_MARKER_RST2	= 0xd2, // リスタートマーカー2, data 無し
	JFIF_MARKER_RST3	= 0xd3, // リスタートマーカー3, data 無し
	JFIF_MARKER_RST4	= 0xd4, // リスタートマーカー4, data 無し
	JFIF_MARKER_RST5	= 0xd5, // リスタートマーカー5, data 無し
	JFIF_MARKER_RST6	= 0xd6,	// リスタートマーカー6, data 無し
	JFIF_MARKER_RST7	= 0xd7, // リスタートマーカー7, data 無し
	JFIF_MARKER_SOI		= 0xd8,	// SOI, data 無し
	JFIF_MARKER_EOI		= 0xd9,	// EOI, data 無し
	JFIF_MARKER_SOS		= 0xDA,	// , スキャンの開始セグメント
	JFIF_MARKER_DQT		= 0xDB,	// , 量子化テーブル定義セグメント
	JFIF_MARKER_DNL		= 0xDC,	// 行数を定義する
	JFIF_MARKER_DRI		= 0xDD,	// リスタートの間隔を定義する
	JFIF_MARKER_DHP		= 0xDE,	// 階層行数を定義する(ハイアラーキカル方式)
	JFIF_MARKER_EXP		= 0xDF,	// 標準イメージの拡張参照
	JFIF_MARKER_APP0	= 0xE0,	// JFIF(JPEGはこれ！), タイプ０のアプリケーションセグメント
	JFIF_MARKER_APP1	= 0xE1,	// Exif、http:, タイプ１のアプリケーションセグメント
	JFIF_MARKER_APP2	= 0xE2,	// ICC_P, タイプ２のアプリケーションセグメント
	JFIF_MARKER_APP3	= 0xE3,	// , タイプ３のアプリケーションセグメント
	JFIF_MARKER_APP4	= 0xE4,	// , タイプ４のアプリケーションセグメント
	JFIF_MARKER_APP5	= 0xE5,	// , タイプ５のアプリケーションセグメント
	JFIF_MARKER_APP6	= 0xE6,	// , タイプ６のアプリケーションセグメント
	JFIF_MARKER_APP7	= 0xE7,	// , タイプ７のアプリケーションセグメント
	JFIF_MARKER_APP8	= 0xE8,	// , タイプ８のアプリケーションセグメント
	JFIF_MARKER_APP9	= 0xE9,	// , タイプ９のアプリケーションセグメント
	JFIF_MARKER_APP10	= 0xEA,	// , タイプ１０のアプリケーションセグメント
	JFIF_MARKER_APP11	= 0xEB,	// , タイプ１１のアプリケーションセグメント
	JFIF_MARKER_APP12	= 0xEC,	// Ducky, タイプ１２のアプリケーションセグメント
	JFIF_MARKER_APP13	= 0xED,	// Photo, タイプ１３のアプリケーションセグメント
	JFIF_MARKER_APP14	= 0xEE,	// Adobe, タイプ１４のアプリケーションセグメント
	JFIF_MARKER_APP15	= 0xEF,	// , タイプ１５のアプリケーションセグメント
	JFIF_MARKER_JPG0	= 0xF0,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG1	= 0xF1,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG2	= 0xF2,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG3	= 0xF3,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG4	= 0xF4,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG5	= 0xF5,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG6	= 0xF6,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG7	= 0xF7,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG8	= 0xF8,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG9	= 0xF9,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG10	= 0xFA,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG11	= 0xFB,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG12	= 0xFC,	// JPEGの拡張のための予備
	JFIF_MARKER_JPG13	= 0xFD,	// JPEGの拡張のための予備
	JFIF_MARKER_COM		= 0xFE,	// コメントセグメント
} jfif_marker_t;

typedef enum dct_mode {
	DCT_MODE_ISLOW,
	DCT_MODE_IFAST,
	DCT_MODE_FLOAT,
} dct_mode_t;

class VideoFrame;
class UVCStream;

// 受信した映像フレーム毎に呼び出されるコールバック
typedef int (*video_stream_callback_t)(VideoFrame *frame, void *frame_callback_args);
// 静止画キャプチャ時に呼び出されるコールバック
typedef int (*still_capture_callback_t)(VideoFrame *frame, void *frame_callback_args);

// このクラスでの変換処理は今の実装では全てCPU上で実行されるのであまり早くない
// 可能な限りGPUで処理するようにすべき
// RGB888やBGR888は1ピクセル3バイトで2/4バイト境界にアライメント出来きず遅いので
// 使うべきではない
/**
*
*/
class VideoFrame {
friend UVCStream;
private:
	tjhandle _jpegDecompressor;
	dct_mode_t _dct_mode;
protected:
	raw_frame_t _frame_type;
	uint32_t _width;
	uint32_t _height;
	uint32_t _step;
	uint32_t _pixelBytes;
	size_t _actual_bytes;
	nsecs_t _presentation_time_us;
	// 受信時のシステム時刻[マイクロ秒]保持用, #update_presentationtime_usを呼んだ時に設定される
	nsecs_t _received_sys_time_us;
	uint32_t _sequence;
	uint32_t _flags;
	uint32_t _pts;	// ペイロードヘッダーのPresentation Time Stamp, イメージセンサーから映像を取得開始した時のクロック値
	// stcとsofをあわせてSCR(Source Clock Reference)
	uint32_t _stc;	// Source Time Clock, 取得したイメージをUSBバスにセットし始めたときのクロック値
	uint32_t _sof;	// 1KHz SOF token counter, stcを取得したときの１KHｚ SOFカウンタの値
	size_t _packet_id;
	// ペイロード中のマイクロフレームの数
	int _frame_num_in_payload;
	std::vector<uint8_t> _frame;
	std::vector<uint8_t> _work;
	uint32_t _option;
	static int appendAux(VideoFrame &dest, const uint8_t *data, const size_t &len);
public:
	VideoFrame(const dct_mode_t &dct_mode = DEFAULT_DCT_MODE);
	VideoFrame(const uint32_t &bytes, const dct_mode_t &dct_mode = DEFAULT_DCT_MODE);	// サイズ指定付きコンストラクタ
	VideoFrame(const uint32_t &width, const uint32_t &height,
		const raw_frame_t &frame_type, const dct_mode_t &dct_mode = DEFAULT_DCT_MODE); // サイズ指定付きコンストラクタ
	VideoFrame(VideoFrame &src);		// コピーコンストラクタ
	virtual ~VideoFrame();				// デストラクタ
	inline uint8_t &operator[](uint32_t ix) { return _frame[ix]; }
	inline const uint8_t &operator[](uint32_t ix) const { return _frame[ix]; }
	VideoFrame &operator=(const VideoFrame &src);	// 代入演算子(ディープコピー)

	/**
	 * フレームヘッダーをチェックする
	 * 今のところチェックできるのはMJPEGのみ
	 * @return	USB_SUCCESS:正常,
	 *			USB_ERROR_REALLOC:フレームサイズが異なっていたが実際のサイズに修正した,
	 *			その他:修正しようとしたがメモリが足りなかった・思っているのとフレームフォーマットが違ったなど
	 */
	int check_header();
	/**
	 * サニタリーチェック、
	 * MJPEGのみ、ヘッダーのチェックに加えてJFIFマーカーを手繰ってフォーマットが正しいかどうかを確認
	 * @return	USB_SUCCESS:正常
	 *			USB_ERROR_REALLOC:フレームサイズが異なっていたが実際のサイズに修正した,
	 *			その他:修正しようとしたがメモリが足りなかった・思っているのとフレームフォーマットが違ったなど
	 */
	int check_sanitary();
	/**
	 * destへフレームデータをコピーする
	 * 代入演算子と違って必要であればフレームフォーマットの変換を行う
	 * @return (変換)コピーできれば0を返す
	 */
	int copy_to(VideoFrame &dest);

	/**
	 * バッファサイズを変更
	 * サイズが大きくなる時以外は実際のバッファのサイズ変更はしない
	 * width, height, pixelBytes, stepは変更しない
	 * @param bytes バッファサイズ
	 * @return actual_bytes この#resizeだけactual_bytesを返すので注意, 他は成功したらUSB_SUCCESS(0), 失敗したらエラーコード
	 */
	size_t resize(const size_t &new_bytes);
	/**
	 * バッファサイズを変更
	 * @param new__width
	 * @param new__height
	 * @param new__pixel_bytes 1ピクセルあたりのバイト数, 0なら実際のバッファサイズの変更はせずにwidth, height等を更新するだけ
	 * @return サイズ変更できればUSB_SUCCESS(0), それ以外ならエラー値を返す
	 */
	int resize(const uint32_t &new_width, const uint32_t &new_height,
		const uint32_t &new_pixel_bytes);
	/**
	 * バッファサイズを変更
	 * @param other widthと高さはこのVideoFrameインスタンスのものを使う
	 * @param new__frame_type フレームの種類(raw_frame_t)
	 * @return サイズ変更できればUSB_SUCCESS(0), それ以外ならエラー値を返す
	 */
	int resize(VideoFrame &other, const raw_frame_t &new_frame_type);
	/** 実際のバッファ自体を開放して保持しているデータサイズもクリアする */
	void recycle();
	/** 保持しているデータサイズをクリアする, 実際のバッファ自体はそのまま */
	inline void clear() { _actual_bytes = 0; };

	inline const raw_frame_t &frame_type() const { return _frame_type; };
	void set_format(const uint32_t &new_width, const uint32_t &new_height,
		const raw_frame_t &new_frame_type);
	void set_format(const raw_frame_t &new_format);
	// このVideoFrameオブジェクトが保持しているデータがmjpegの時に、
	// エンベデッドされている外部ストリームのデータがあればそれを取得する
	int extract_aux_frame(VideoFrame &dest,
		const raw_frame_t &target_frame_type = RAW_FRAME_H264);
	const size_t &actual_bytes() { return _actual_bytes; };
	inline const size_t size() { return _frame.size(); };
	inline void setFrame(const uint8_t *data, const size_t &bytes) {
		resize(bytes);
		memcpy(&_frame[0], data, bytes);
	}
	inline uint8_t *frame() { return &_frame[0]; };
	inline const uint32_t &width() const { return _width; };
	inline const uint32_t &height() const { return _height; };
	inline const uint32_t &step() const { return _step; };

	inline const nsecs_t &update_presentationtime_us(
		const uint32_t &sequence, const nsecs_t &presentationtime_us = 0) {

		_sequence = sequence;
		_received_sys_time_us = systemTime() / 1000LL;
		if (presentationtime_us) {
			_presentation_time_us = presentationtime_us;
		} else {
			_presentation_time_us = _received_sys_time_us;
		}
		return _presentation_time_us;
	};
	inline const nsecs_t &presentationtime_us() const { return _presentation_time_us; };
	inline void presentationtime_us(const nsecs_t &presentationtime_us) {
		_presentation_time_us = presentationtime_us;
	};
	
	// 受信時のシステム時刻[マイクロ秒], #update_presentationtime_usを呼んだ時に設定される
	inline const nsecs_t  &received_sys_time_us() const { return _received_sys_time_us; };
	inline const uint32_t &sequence() const { return _sequence; };
	/** メージセンサーから映像を取得開始した時のクロック値 */
	inline const uint32_t &pts() const { return _pts; };
	/** 取得したイメージをUSBバスにセットし始めたときのクロック値 */
    inline const uint32_t &stc() const { return _stc; };
    /** stcを取得したときの１KHｚ SOFカウンタの値 */
    inline const uint32_t &sof() const { return _sof; };
	inline void packet_id(const size_t &packet_id) { _packet_id = packet_id; };
    inline const size_t &packet_id() const { return _packet_id; };
    inline void frame_num_in_payload(const int &frame_num_in_payload) {
    	_frame_num_in_payload = frame_num_in_payload;
    };
    inline const int &frame_num_in_payload() const { return _frame_num_in_payload; };
    int set_header_info(const uint32_t &pts, const uint32_t &stc, const uint32_t &sof);
	int set_header_info(const VideoFrame &other);

	const bool start_with_annexb();
	const bool is_iframe();
	inline const uint32_t &flags() const { return _flags; };
	inline void flags(const uint32_t &flags) { _flags = flags; };
	inline const uint32_t &option() const { return _option; };
	inline void option(const uint32_t &option) { _option = option; };
	// _presentationtime_us, _received_sys_time_us, sequence, _flags等をコピーする
	inline void set_attribute(const VideoFrame &src) {
		_presentation_time_us = src._presentation_time_us;
		_received_sys_time_us = src._received_sys_time_us;
		_sequence = src._sequence;
		_flags = src._flags;
		_option = src._option;
    	_pts = src._pts;
    	_stc = src._stc;
    	_sof = src._sof;
    	_packet_id = src._packet_id;
    	_frame_num_in_payload = src._frame_num_in_payload;
	};
	// annexBフォーマットの時に指定した位置からnalユニットを探す
	// 最初に見つかったnalユニットの先頭オフセットをoffsetに返す
	// annexBフォーマットでない時、見つからない時は負を返す
	// 見つかった時はnal_unit_type_tのいずれかを返す
	const int nal_unit_type(size_t &offset);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* UVC_FRAME_H_ */
