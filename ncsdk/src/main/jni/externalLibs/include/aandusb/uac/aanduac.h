/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef AANDUAC_H_
#define AANDUAC_H_

#include <linux/usb/audio.h>
#include <linux/usb/midi.h>
#include "audio-v2.h"
// core
#include "core/aandusb.h"

namespace serenegiant {
namespace usb {
namespace uac {

// UAC1
// UACのClass Specific descriptorの先頭部分
typedef struct uac_descriptor_header {
	 __u8 bLength;
	 __u8 bDescriptorType;
	 __u8 bDescriptorSubType;
} __attribute__ ((packed)) uac_descriptor_header_t;

typedef struct uac1_ac_header_descriptor				uac1_ac_header_descriptor_t;
typedef struct uac_input_terminal_descriptor			uac1_input_terminal_descriptor_t;
/*struct uac_input_terminal_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bTerminalID;
	__le16 wTerminalType;
	__u8 bAssocTerminal;
	__u8 bNrChannels;				// チャネル数
	__le16 wChannelConfig;			// チャネルコンフィグクラスター
	__u8 iChannelNames;
	__u8 iTerminal;
} __attribute__ ((packed)); */
typedef struct uac1_output_terminal_descriptor			uac1_output_terminal_descriptor_t;
/*struct uac_input_terminal_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bTerminalID;
	__le16 wTerminalType;
	__u8 bAssocTerminal;
	__u8 bNrChannels;				// チャネル数
	__le16 wChannelConfig;			// チャネルコンフィグクラスター
	__u8 iChannelNames;
	__u8 iTerminal;
} __attribute__ ((packed)); */
typedef struct uac_mixer_unit_descriptor				uac1_mixer_unit_descriptor_t;
/*struct uac_mixer_unit_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bUnitID;
	__u8 bNrInPins;
	__u8 baSourceID[];
} __attribute__ ((packed)); */
typedef struct uac_selector_unit_descriptor				uac1_selector_unit_descriptor_t;
/*struct uac_selector_unit_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bUnitID;
	__u8 bNrInPins;
	__u8 baSourceID[];
} __attribute__ ((packed)); */
typedef struct uac_feature_unit_descriptor				uac1_feature_unit_descriptor_t;
/*struct uac_selector_unit_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bUnitID;
	__u8 bNrInPins;
	__u8 baSourceID[];
} __attribute__ ((packed)); */
typedef struct uac_processing_unit_descriptor			uac1_processing_unit_descriptor_t;
/*struct uac_processing_unit_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bUnitID;
	__u16 wProcessType;
	__u8 bNrInPins;
	__u8 baSourceID[];
} __attribute__ ((packed)); */

typedef struct uac_extension_unit_descriptor {
	 __u8 bLength;
	 __u8 bDescriptorType;
	 __u8 bDescriptorSubType;
	 __u8 bUnitID;
	 __u16 wExtensionCode;
	 __u8 bNrInPins;
	 __u8 baSourceID[0];
	 __u8 bNrChannels;
	 __u16 wChannelConfig;
	 __u8 iChannelNames;
	 __u8 bControlSize;
	 __u8 bmControls[0];
	 __u8 iExtension;
} __attribute__ ((packed)) uac1_extension_unit_descriptor_t;

typedef struct uac1_as_header_descriptor	uac1_as_header_descriptor_t;
/*struct uac1_as_header_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;	// uac_format_type_t
	__u8 bTerminalLink;
	__u8 bDelay;
	__le16 wFormatTag;			// uac_raw_format_t
} __attribute__ ((packed)); */

// uac_format_type_n_xxxの先頭部分
typedef struct uac_format_descriptor_header {
	 __u8 bLength;
	 __u8 bDescriptorType;
	 __u8 bDescriptorSubType;
	 __u8 bFormatType;
} __attribute__ ((packed)) uac_format_descriptor_header_t;

// uac_format_type_i_xxxの共通部分
typedef struct uac_format_i_common_descriptor {
	 __u8 bLength;
	 __u8 bDescriptorType;
	 __u8 bDescriptorSubType;
	 __u8 bFormatType;
	 __u8 bNrChannels;		// チャネル数
	 __u8 bSubframeSize;	// 音声サンプリング1つ(1チャネル)当たりのバイト数, 1,2,3,4[バイト]
	 __u8 bBitResolution;	// 音声サンプリング1つ(1チャネル)当たりの有効なビット数 bBitResolution ≦  bSubframeSize ✕ 8
	 __u8 bSamFreqType;		// 0の時は連続(上下限は続く3バイト✕2)、1-255の時は離散サンプリングの周期[ナノ秒]を示すインデックスの数n(実際のデータは続く3バイト✕n個)
} __attribute__ ((packed)) uac_format_i_common_descriptor_t;
// AudioサブフレームのサイズはbSubframeSize[バイト]
// AudioフレームサイズはbNrChannels ✕ bSubframeSize[バイト]
// 各Audioフレーム内でのAudioサブフレーム(チャネル毎のデータ)の並ぶ順番はAudioチャネルクラスター定義で定義されている順番
// AudioストリームサイズはAudioフレームサイズの整数倍
// データが無い時はTransferデリミタ(長さゼロのパケットかアイソクロナス転送のパケット無し)

typedef struct uac_format_type_i_continuous_descriptor	uac_format_i_continuous_descriptor_t;
typedef struct uac_format_type_i_discrete_descriptor	uac_format_i_discrete_descriptor_t;
typedef struct uac_format_type_i_ext_descriptor			uac_format_i_ext_descriptor_t;

// uac_format_type_i_xxxの共用体
typedef struct uac_format_i_descriptor {
	union {
		uac_format_descriptor_header_t header;
		uac_format_i_common_descriptor_t common;
		uac_format_i_continuous_descriptor_t contiuous;
		uac_format_i_discrete_descriptor_t discreate;
		uac_format_i_ext_descriptor_t ext;	// commonとちょっと違うので注意
	};
} __attribute__ ((packed)) uac_format_i_descriptor_t;

// uac_format_type_ii_xxxの共通部分
typedef struct uac_format_ii_common_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;
	__u8 bFormatType;
	__le16 wMaxBitRate;
	__le16 wSamplesPerFrame;
	__u8 bSamFreqType;
} __attribute__((packed)) uac_format_ii_common_descriptor_t;

typedef struct uac_format_ii_continuous_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__u8 bDescriptorSubType;	// uac_format_type_t
	__u8 bFormatType;
	__le16 wMaxBitRate;
	__le16 wSamplesPerFrame;
	__u8 bSamFreqType;
	__u8 tLowerSamFreq[3];
	__u8 tUpperSamFreq[3];
} __attribute__((packed)) uac_format_ii_continuous_descriptor_t;

typedef struct uac_format_type_ii_discrete_descriptor uac_format_ii_discrete_descriptor_t;
typedef struct uac_format_type_ii_ext_descriptor uac_format_ii_ext_descriptor_t;

// uac_format_type_ii_xxxの共用体
typedef struct uac_format_ii_descriptor {
	union {
		uac_format_descriptor_header_t header;
		uac_format_ii_common_descriptor_t common;
		uac_format_ii_continuous_descriptor_t contiuous;
		uac_format_ii_discrete_descriptor_t discreate;
		uac_format_ii_ext_descriptor_t ext;
	};
} __attribute__ ((packed)) uac_format_ii_descriptor_t;

// uac_format_type_n_xxxを一括で取り扱うための共用体
typedef struct uac_format_descriptor {
	union {
		uac_format_descriptor_header_t header;
		uac_format_i_descriptor_t type_i;
		uac_format_ii_descriptor_t type_ii;
		// type_iiiは?
	};
} __attribute__ ((packed)) uac_format_descriptor_t;

typedef struct uac_iso_endpoint_descriptor				uac_iso_endpoint_descriptor_t;
/*struct uac_iso_endpoint_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;			// CS_ENDPOINT
	__u8 bDescriptorSubType;		// EP_GENERAL
	__u8 bmAttributes;				// サポートされるコントロール
									// D0: Sampling Frequency
									// D1: Pitch
									// D6..2: 予約
									// D7: MaxPacketsOnly, 1ならwMaxPacketSizeのパケットが必要
	__u8 bLockDelayUnits;			// wLockDelayの単位、多分いつも0x00
									// 0: 未定義
									// 1: ミリ秒
									// 2: デコードしたPCMのサンプル数
	__le16 wLockDelay;				// endpointがロックされる期間(単位はbLockDelayUnits), 多分いつも0x0000
} __attribute__((packed)); */

typedef struct uac1_status_word							uac1_status_word_t;

// UAC2
typedef struct uac2_format_type_i_descriptor			uac2_format_type_i_descriptor_t;
typedef struct uac2_ac_header_descriptor				uac2_ac_header_descriptor_t;
typedef struct uac_clock_source_descriptor				uac2_clock_source_descriptor_t;
typedef struct uac_clock_selector_descriptor			uac2_clock_selector_descriptor_t;
typedef struct uac_clock_multiplier_descriptor			uac2_clock_multiplier_descriptor_t;
typedef struct uac2_input_terminal_descriptor			uac2_input_terminal_descriptor_t;
typedef struct uac2_output_terminal_descriptor			uac2_output_terminal_descriptor_t;
typedef struct uac2_feature_unit_descriptor				uac2_feature_unit_descriptor_t;
typedef struct uac2_as_header_descriptor				uac2_as_header_descriptor_t;
typedef struct uac2_iso_endpoint_descriptor				uac2_iso_endpoint_descriptor_t;
typedef struct uac2_interrupt_data_msg					uac2_interrupt_data_msg_t;

// インターフェースディスクリプタのbInterfaceSubClassの値
typedef enum uac_subclass {
	UAC_SC_UNDEFINED = 0x00,
	UAC_SC_AUDIOCONTROL = USB_SUBCLASS_AUDIOCONTROL,
	UAC_SC_AUDIOSTREAMING = USB_SUBCLASS_AUDIOSTREAMING,
	UAC_SC_MIDISTREAMING = USB_SUBCLASS_MIDISTREAMING,
} uac_subclass_t;

// クラス固有インターフェースディスクリプタのディスクリプタサブタイプ
// uac_descriptor_header_t#bDescriptorSubType
typedef enum uac_csi_subtype {
	UAC_CSI_TYPE_UNDEFINED = 0x00,							// 0x00
	UAC_CSI_TYPE_HEADER = UAC_HEADER, 						// 0x01
	UAC_CSI_TYPE_INPUT_TERMINAL = UAC_INPUT_TERMINAL,		// 0x02
	UAC_CSI_TYPE_OUTPUT_TERMINAL = UAC_OUTPUT_TERMINAL,		// 0x03
	UAC_CSI_TYPE_MIXER_UNIT = UAC_MIXER_UNIT,				// 0x04
	UAC_CSI_TYPE_SELECTOR_UNIT = UAC_SELECTOR_UNIT,			// 0x05
	UAC_CSI_TYPE_FEATURE_UNIT = UAC_FEATURE_UNIT,			// 0x06
	UAC1_CSI_TYPE_PROCESSING_UNIT = UAC1_PROCESSING_UNIT,	// 0x07
	UAC1_CSI_TYPE_EXTENSION_UNIT = UAC1_EXTENSION_UNIT,		// 0x08
} uac_csi_subtype_t;

// AudioStreamのディスクリプタサブタイプ
// uac_descriptor_header_t#bDescriptorSubType
typedef enum uac_as_subtype {
	UAC_AS_TYPE_UNDEFINED = 0x00,							// 0x00
	UAC_AS_TYPE_GENERAL = UAC_AS_GENERAL,					// 0x01
	UAC_AS_TYPE_FORMAT_TYPE = UAC_FORMAT_TYPE, 				// 0x02
	UAC_AS_TYPE_FORMAT_SPECIFIC = UAC_FORMAT_SPECIFIC,		// 0x03
} uac_as_subtype_t;

// インプットターミナルの種類
typedef enum uac_input_terminal_type {
	UAC_IT_TYPE_UNDEFINED = UAC_INPUT_TERMINAL_UNDEFINED,							// 0x200
	UAC_IT_TYPE_MICROPHONE = UAC_INPUT_TERMINAL_MICROPHONE,							// 0x201
	UAC_IT_TYPE_DESKTOP_MICROPHONE = UAC_INPUT_TERMINAL_DESKTOP_MICROPHONE,			// 0x202
	UAC_IT_TYPE_PERSONAL_MICROPHONE = UAC_INPUT_TERMINAL_PERSONAL_MICROPHONE,		// 0x203
	UAC_IT_TYPE_OMNI_DIR_MICROPHONE = UAC_INPUT_TERMINAL_OMNI_DIR_MICROPHONE,		// 0x204
	UAC_IT_TYPE_MICROPHONE_ARRAY = UAC_INPUT_TERMINAL_MICROPHONE_ARRAY,				// 0x025
	UAC_IT_TYPE_PROC_MICROPHONE_ARRAY = UAC_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY,	// 0x206
} uac_input_terminal_type_t;

// アウトプットターミナルの種類
typedef enum uac_output_termial_type {
	UAC_OT_TYPE_UNDEFINED = UAC_OUTPUT_TERMINAL_UNDEFINED,									// 0x300,
	UAC_OT_TYPE_SPEAKER = UAC_OUTPUT_TERMINAL_SPEAKER, 										// 0x301,
	UAC_OT_TYPE_HEADPHONES = UAC_OUTPUT_TERMINAL_HEADPHONES, 								// 0x302,
	UAC_OT_TYPE_HEAD_MOUNTED_DISPLAY_AUDIO = UAC_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO,// 0x303,
	UAC_OT_TYPE_DESKTOP_SPEAKER = UAC_OUTPUT_TERMINAL_DESKTOP_SPEAKER, 						// 0x304,
	UAC_OT_TYPE_ROOM_SPEAKER = UAC_OUTPUT_TERMINAL_ROOM_SPEAKER, 							// 0x305,
	UAC_OT_TYPE_COMMUNICATION_SPEAKER = UAC_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER, 			// 0x306,
	UAC_OT_TYPE_LOW_FREQ_EFFECTS_SPEAKER = UAC_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER, 	// 0x307,
} uac_output_termial_type_t;

// プロセッシングユニットの種類
typedef enum uac_processing_unit_type {
	UAC_PU_TYPE_UNDEFINED = UAC_PROCESS_UNDEFINED,				// 0x00
	UAC_PU_TYPE_UP_DOWNMIX = UAC_PROCESS_UP_DOWNMIX,			// 0x01
	UAC_PU_TYPE_DOLBY_PROLOGIC = UAC_PROCESS_DOLBY_PROLOGIC,	// 0x02
	UAC_PU_TYPE_STEREO_EXTENDER = UAC_PROCESS_STEREO_EXTENDER,	// 0x03
	UAC_PU_TYPE_REVERB = UAC_PROCESS_REVERB,					// 0x04
	UAC_PU_TYPE_CHORUS = UAC_PROCESS_CHORUS,					// 0x05
	UAC_PU_TYPE_DYN_RANGE_COMP = UAC_PROCESS_DYN_RANGE_COMP,	// 0x06
} uac_processing_unit_type_t;

// フォーマットの種類
// uac_format_type_n_xxx#bFormatType
typedef enum uac_format_type {
	UAC_FORMAT_UNDEFINED = UAC_FORMAT_TYPE_UNDEFINED,
	UAC_FORMAT_I = UAC_FORMAT_TYPE_I,
	UAC_FORMAT_II = UAC_FORMAT_TYPE_II,
	UAC_FORMAT_III = UAC_FORMAT_TYPE_III,
	UAC_EXT_FORMAT_I = UAC_EXT_FORMAT_TYPE_I,
	UAC_EXT_FORMAT_II = UAC_EXT_FORMAT_TYPE_II,
	UAC_EXT_FORMAT_III = UAC_EXT_FORMAT_TYPE_III,
} uac_format_type_t;

// エンドポイントの種類
// uac_descriptor_header#bDescriptorSubType
typedef enum uac_endpoint_type {
	UAC_ENDPOINT_TYPE_UNDEFINED = 0x00,
	UAC_ENDPOINT_TYPE_GENERAL = UAC_EP_GENERAL, // 0x01
} uac_endpoint_type_t;

// フォーマットの種類詳細(UAC_FORMAT_Iの場合)
//  uac1_as_header_descriptor->wFormatTagの値
typedef enum uac_format_type1 {
	UAC_FORMAT_I_UNDEFINED = UAC_FORMAT_TYPE_I_UNDEFINED,	// 0x0000
	UAC_FORMAT_I_PCM = UAC_FORMAT_TYPE_I_PCM,				// 0x0001
	UAC_FORMAT_I_PCM8 = UAC_FORMAT_TYPE_I_PCM8,				// 0x0002
	UAC_FORMAT_I_IEEE_FLOAT = UAC_FORMAT_TYPE_I_IEEE_FLOAT,	// 0x0003
	UAC_FORMAT_I_ALAW = UAC_FORMAT_TYPE_I_ALAW,				// 0x0004
	UAC_FORMAT_I_MULAW = UAC_FORMAT_TYPE_I_MULAW,			// 0x0005
} uac_format_type1_t;

// フォーマットの種類詳細(UAC_FORMAT_IIの場合)
//  uac1_as_header_descriptor->wFormatTagの値
typedef enum uac_format_type2 {
	UAC_FORMAT_II_MPEG = UAC_FORMAT_TYPE_II_MPEG,			// 0x1001
	UAC_FORMAT_II_AC3 = UAC_FORMAT_TYPE_II_AC3,				// 0x1002
} uac_format_type2_t;

// フォーマットの種類詳細(UAC_FORMAT_IIIの場合)
//  uac1_as_header_descriptor->wFormatTagの値
typedef enum uac_format_type3 {
	UAC_FORMAT_III_IEC1937_AC3 = UAC_FORMAT_TYPE_III_IEC1937_AC3,							// 0x2001
	UAC_FORMAT_III_IEC1937_MPEG1_LAYER1 = UAC_FORMAT_TYPE_III_IEC1937_MPEG1_LAYER1,			// 0x2002
	UAC_FORMAT_III_IEC1937_MPEG2_NOEXT = UAC_FORMAT_TYPE_III_IEC1937_MPEG2_NOEXT,			// 0x2003
	UAC_FORMAT_III_IEC1937_MPEG2_EXT = UAC_FORMAT_TYPE_III_IEC1937_MPEG2_EXT,				// 0x2004
	UAC_FORMAT_III_IEC1937_MPEG2_LAYER1_LS = UAC_FORMAT_TYPE_III_IEC1937_MPEG2_LAYER1_LS,	// 0x2005
	UAC_FORMAT_III_IEC1937_MPEG2_LAYER23_LS = UAC_FORMAT_TYPE_III_IEC1937_MPEG2_LAYER23_LS,	// 0x2006
} uac_format_type3_t;

// フォーマットの種類詳細
//  uac1_as_header_descriptor->wFormatTagの値
typedef enum uac_raw_format {
	UAC_RAW_FORMAT_I_UNDEFINED = UAC_FORMAT_I_UNDEFINED,	// 0x0000
	UAC_RAW_FORMAT_I_PCM = UAC_FORMAT_I_PCM,				// 0x0001
	UAC_RAW_FORMAT_I_PCM8 = UAC_FORMAT_I_PCM8,				// 0x0002
	UAC_RAW_FORMAT_I_IEEE_FLOAT = UAC_FORMAT_I_IEEE_FLOAT,	// 0x0003
	UAC_RAW_FORMAT_I_ALAW = UAC_FORMAT_I_ALAW,				// 0x0004
	UAC_RAW_FORMAT_I_MULAW = UAC_FORMAT_I_MULAW,			// 0x0005
//
	UAC_RAW_FORMAT_II_MPEG = UAC_FORMAT_II_MPEG,			// 0x1001
	UAC_RAW_FORMAT_II_AC3 = UAC_FORMAT_II_AC3,				// 0x1002
//
	UAC_RAW_FORMAT_III_IEC1937_AC3 = UAC_FORMAT_III_IEC1937_AC3,							// 0x2001
	UAC_RAW_FORMAT_III_IEC1937_MPEG1_LAYER1 = UAC_FORMAT_III_IEC1937_MPEG1_LAYER1,			// 0x2002
	UAC_RAW_FORMAT_III_IEC1937_MPEG2_NOEXT = UAC_FORMAT_III_IEC1937_MPEG2_NOEXT,			// 0x2003
	UAC_RAW_FORMAT_III_IEC1937_MPEG2_EXT = UAC_FORMAT_III_IEC1937_MPEG2_EXT,				// 0x2004
	UAC_RAW_FORMAT_III_IEC1937_MPEG2_LAYER1_LS = UAC_FORMAT_III_IEC1937_MPEG2_LAYER1_LS,	// 0x2005
	UAC_RAW_FORMAT_III_IEC1937_MPEG2_LAYER23_LS = UAC_FORMAT_III_IEC1937_MPEG2_LAYER23_LS,	// 0x2006
} uac_raw_format_t;

// 機能ユニットの種類
typedef enum uac_feature_unit_type {
	UAC_FU_TYPE_UNDEFINED = 0x00,								// 0x00
	UAC_FU_TYPE_MUTE = UAC_FU_MUTE,								// 0x01
	UAC_FU_TYPE_VOLUME = UAC_FU_VOLUME,							// 0x02
	UAC_FU_TYPE_BASS = UAC_FU_BASS,								// 0x03
	UAC_FU_TYPE_MID = UAC_FU_MID,								// 0x04
	UAC_FU_TYPE_TREBLE = UAC_FU_TREBLE,							// 0x05
	UAC_FU_TYPE_GRAPHIC_EQUALIZER = UAC_FU_GRAPHIC_EQUALIZER,	// 0x06
	UAC_FU_TYPE_AUTOMATIC_GAIN = UAC_FU_AUTOMATIC_GAIN,			// 0x07
	UAC_FU_TYPE_DELAY = UAC_FU_DELAY,							// 0x08
	UAC_FU_TYPE_BASS_BOOST = UAC_FU_BASS_BOOST,					// 0x09
	UAC_FU_TYPE_LOUDNESS = UAC_FU_LOUDNESS,						// 0x0a
} uac_feature_unit_type_t;

typedef enum uac1_status_type {
	UAC1_STATUS_ORIG_MASK = UAC1_STATUS_TYPE_ORIG_MASK,
	UAC1_STATUS_ORIG_AUDIO_CONTROL_IF = UAC1_STATUS_TYPE_ORIG_AUDIO_CONTROL_IF,
	UAC1_STATUS_ORIG_AUDIO_STREAM_IF = UAC1_STATUS_TYPE_ORIG_AUDIO_STREAM_IF,
	UAC1_STATUS_ORIG_AUDIO_STREAM_EP = UAC1_STATUS_TYPE_ORIG_AUDIO_STREAM_EP,
	UAC1_STATUS_IRQ_PENDING = UAC1_STATUS_TYPE_IRQ_PENDING,
	UAC1_STATUS_MEM_CHANGED = UAC1_STATUS_TYPE_MEM_CHANGED,
} uac1_status_type_t;

// wChannelConfigの定義内容(ビットフィールド)
// ここで定義されるビットの上から順にAudioフレーム内で前から順に並ぶ
// 例えばD0,D1がONならば前からL→Rの順にAudioサブフレームが並ぶ
typedef enum channel_config_flag {
	CHANNEL_CONFIG_LEFT_FRONT	= 0x0001,				// D0, L
	CHANNEL_CONFIG_RIGHT_FRONT	= 0x0002,				// D1, R
	CHANNEL_CONFIG_CENTER_FRONT = 0x0004,				// D2, C
	CHANNEL_CONFIG_LOW_FREQUENCY_ENHANCEMENT = 0x0008,	// D3, LFE
	CHANNEL_CONFIG_LEFT_SURROUND = 0x0010,				// D4, Ls
	CHANNEL_CONFIG_RIGHT_SURROUND = 0x0020,				// D5, Rs
	CHANNEL_CONFIG_LEFT_OF_CENTER = 0x0040,				// D6, Lc
	CHANNEL_CONFIG_RIGHT_OF_CENTER = 0x0080,			// D7, Rc
	CHANNEL_CONFIG_SURROUND = 0x0100,					// D8, S
	CHANNEL_CONFIG_SIDE_LEFT = 0x0200,					// D9, SL
	CHANNEL_CONFIG_SIDE_RIGHT = 0x0400,					// D10, SR
	CHANNEL_CONFIG_TOP = 0x0800,						// D11, T
														// D15..12: 予約
} channel_config_flag_t;

// Audioストリームの設定(ネゴシエーション用)
typedef struct uac_stream_ctrl {
	uint8_t bEndpointAddress;		// エンドポイントアドレス endpoint_descriptor_t#bEndpointAddress
	uint8_t bAlternateSetting;		// 代替設定
	size_t wMaxPacketSize;			// 最大パケットサイズ endpoint_descriptor_t#wMaxPacketSize
	uint8_t bInterval;				// 転送インターバル[ミリ秒] endpoint_descriptor_t#bInterval
	uint8_t bTerminalLink;			// ターミナルID
	uac_raw_format_t wFormatTag;	// uac1_as_header_descriptor_t->wFormatTag
	uint8_t bFormatType;			// フォーマットの種類
	uint8_t bNrChannels;			// チャネル数
	uint8_t bSubframeSize;			// 音声サンプリング1つ(1チャネル)当たりのバイト数, 1,2,3,4[バイト]
	uint8_t bBitResolution;			// 音声サンプリング1つ(1チャネル)当たりの有効なビット数 bBitResolution ≦  bSubframeSize ✕ 8
	uint32_t minSamplingFreq;		// 最小サンプリング周波数
	uint32_t maxSamplingFreq;		// 最大サンプリング周波数
	uint8_t packets_per_transfer;	// 1トランスファー当たりのパケットの数(転送インターバルと同じ値)
	size_t total_transfer_size;		// 1トランスファーのバイト数=wMaxPacketSize ✕ packets_per_transfer
	size_t packet_bytes;			// 1パケット当たりのバイト数(bNrChannels * bSubframeSize * bInterval * maxSamplingFreq / 1000)
} uac_stream_ctrl_t;

#define UAC_STREAM_ERR 0x01

// Endpoint Control Selectors
typedef enum uac_ac_ep_ctrl_selector {
	UAC_EP_CTRL_UNDEFINED = 0x00,
	UAC_EP_CTRL_SAMPLING_FREQ = 0x01,
	UAC_EP_CTRL_PITCH = 0x02,
} uac_ac_ep_ctrl_selector_t;

}	// end of namespace uac
}	// end of namespace usb
}	// end of namespace serenegiant

#endif /* AANDUAC_H_ */
