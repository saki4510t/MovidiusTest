/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef USBWEBCAMERAPROJ_UVC_CONTROLLER_H
#define USBWEBCAMERAPROJ_UVC_CONTROLLER_H

// core
#include "core/context.h"
// uvc
#include "uvc_descriptor.h"

namespace serenegiant {
namespace usb {
namespace uvc {

class UVCDevice;
class UVCControl;

/**
 * こっちはUVCControlのラッパー
 */
class UVCController {
private:
	// カメラコントロール・プロセッシングユニット関係
	uint64_t mCtrlSupports;
	uint64_t mPUSupports;

	control_value_bool_t mScanningMode;			//  SET_CUR,  GET_CUR,                            GET_INFO
	control_value_t mExposureMode;				//  SET_CUR,  GET_CUR,                   GET_RES, GET_INFO, GET_DEF
	control_value_t mExposurePriority;			//  SET_CUR,  GET_CUR,                            GET_INFO
	control_value_t mExposureAbs;				// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mExposureRel;				//  SET_CUR,  GET_CUR,                            GET_INFO
	control_value_t mFocus;						// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value2_t mFocusRel;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mFocusSimple;				//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_bool_t mAutoFocus;			//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mIris;						// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mIrisRel;					//  SET_CUR,  GET_CUR,                            GET_INFO
	control_value_t mZoom;						// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value3_t mZoomRel;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value2_t mPanTilt;					// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value4_t mPanTiltRel;				//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_INFO, GET_DEF, GET_RES
	control_value_t mRoll;						// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value2_t mRollRel;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_bool_t mPrivacy;				// (SET_CUR), GET_CUR,                            GET_INFO
//	control_value6_t mWindow;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX,                    GET_DEF
//	control_value5_t mROI;						//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX,                    GET_DEF
//	control_value_t mSelector;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO
	control_value_t mBacklightComp;				//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mBrightness;				//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mContrast;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_bool_t mAutoContrast;			//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mGain;						//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mPowerlineFrequency;		//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mHue;						// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_bool_t mAutoHue;				//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mSaturation;				//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mSharpness;					//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mGamma;						//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mWhiteBlance;				// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_bool_t mAutoWhiteBlance;		//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mWhiteBlanceCompo;			// (SET_CUR), GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_bool_t mAutoWhiteBlanceCompo;	//  SET_CUR,  GET_CUR,                            GET_INFO, GET_DEF
	control_value_t mMultiplier;				//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mMultiplierLimit;			//  SET_CUR,  GET_CUR, GET_MIN, GET_MAX, GET_RES, GET_INFO, GET_DEF
	control_value_t mAnalogVideoStandard;		//            GET_CUR,                            GET_INFO
	control_value_t mAnalogVideoLockState;		//            GET_CUR,                            GET_INFO

	/** カメラコントロール設定の下請け */
	int internal_set_ctrl_value(control_value_t &values, int8_t value,
		paramget_func_i8 get_func, paramset_func_i8 set_func);
	int internal_set_ctrl_value(control_value_t &values, uint8_t value,
		paramget_func_u8 get_func, paramset_func_u8 set_func);
	int internal_set_ctrl_value(control_value_bool_t &values, uint8_t value,
		paramget_func_u8 get_func, paramset_func_u8 set_func);
	int internal_set_ctrl_value(control_value2_t &values, uint8_t value1, uint8_t value2,
		paramget_func_u8u8 get_func, paramset_func_u8u8 set_func);
	int internal_sSet_ctrl_value(control_value2_t &values, int8_t value1, uint8_t value2,
		paramget_func_i8u8 get_func, paramset_func_i8u8 set_func);
	int internal_set_ctrl_value(control_value3_t &values, int8_t value1, uint8_t value2, uint8_t value3,
		paramget_func_i8u8u8 get_func, paramset_func_i8u8u8 set_func);
	int internal_set_ctrl_value(control_value_t &values, int16_t value,
		paramget_func_i16 get_func, paramset_func_i16 set_func);
	int internal_set_ctrl_value(control_value_t &values, uint16_t value,
		paramget_func_u16 get_func, paramset_func_u16 set_func);
	int internal_set_ctrl_value(control_value_t &values, int32_t value,
		paramget_func_i32 get_func, paramset_func_i32 set_func);
	int internal_set_ctrl_value(control_value_t &values, uint32_t value,
		paramget_func_u32 get_func, paramset_func_u32 set_func);
protected:
	Context context;
	UVCDescriptor descriptor;
	UVCDevice *device;
	UVCControl *ctrl;
	UVCController();
	virtual ~UVCController();
	void clear_camera_params();
public:
	// 指定したパスへディスクリプタ等を出力する
	int output_descriptor(const char *path);
	// JSON形式でカメラがサポートする解像度を返す
	char *get_supported_size();

	// コントロール転送を実行
	int control_transfer(const int &requestType, const int &request,
		const int &value, const int &index,
		uint8_t *buffer, const int &length,
		const int &timeout);
	// カメラコントロールでサポートしている機能を取得
	int get_ctrl_supports(uint64_t *supports);
	// プロセッシングユニットでサポートしている機能を取得
	int get_proc_supports(uint64_t *supports);

	// スキャニングモード
	int update_scanning_mode_limit(control_value_bool_t &value);
	int set_scanning_mode(const int &mode);
	int get_scanning_mode();
	// 露光モード
	int update_exposure_mode_limit(control_value_t &value);
	int set_exposure_mode(const int &mode);
	int get_exposure_mode();
	// 露光優先設定
	int update_exposure_priority_limit(control_value_t &value);
	int set_exposure_priority(const int &priority);
	int get_exposure_priority();
	// 露光設定
	int update_exposure_limit(control_value_t &value);
	int set_exposure(const int &exp_abs);
	int get_exposure();
	// 露光設定(相対) 0:default, 1: +Ev, 0xff: -Ev
	int update_exposure_rel_limit(control_value_t &value);
	int set_exposure_rel(const int &exp_rel);
	int get_exposure_rel();
	// オートフォーカス
	int update_auto_docus_limit(control_value_bool_t &value);
	int set_auto_focus(const bool &autoFocus);
	bool get_auto_focus();
	// フォーカス調整
	int update_focus_limit(control_value_t &value);
	int set_focus(const int &focus);
	int get_focus();
	// フォーカス調整(相対)
	// 下位1バイトはbSpeedで符号なし整数, 2バイト目は符号あり整数0:停止, 1:近くへ, 0xff:遠くへ
	int update_focus_rel_limit(control_value2_t &value);
	int set_focus_rel(const int &focus);
	int get_focus_rel();
/*	int update_focus_simple_limit(int &min, int &max, int &def);
	int set_focus_simple(int focus);
	int get_focus_simple(); */
	// オートホワイトバランス
	int update_auto_white_blance_limit(control_value_bool_t &value);
	int set_auto_white_blance(const bool &autoWhiteBlance);
	bool get_auto_white_blance();
	// ホワイトバランス色温度調整
	int update_white_blance_limit(control_value_t &value);
	int set_white_blance(const int &gain);
	int get_white_blance();
	// 明るさ調整
	int update_brightness_limit(control_value_t &value);
	int set_brightness(const short &brightness);
	int get_brightness();
	// コントラスト調整
	int update_contrast_limit(control_value_t &value);
	int set_contrast(const uint16_t &contrast);
	int get_contrast();
	// シャープネス調整
	int update_sharpness_limit(control_value_t &value);
	int set_sharpness(const int &sharpness);
	int get_sharpness();
	// ゲイン調整
	int update_gain_limit(control_value_t &value);
	int set_gain(const int &gain);
	int get_gain();
	// ガンマ調整
	int update_gamma_limit(control_value_t &value);
	int set_gamma(const int &gamma);
	int get_gamma();
	// 彩度調整
	int update_saturation_limit(control_value_t &value);
	int set_saturation(const int &saturation);
	int get_saturation();
	// 色相調整
	int update_hue_limit(control_value_t &value);
	int set_hue(const int &hue);
	int get_hue();
	// 電源周波数によるチラつき補正
	int update_powerline_frequency_limit(control_value_t &value);
	int set_powerline_frequency(const int &frequency);
	int get_powerline_frequency();
	// ズーム
	int update_zoom_limit(control_value_t &value);
	int set_zoom(const int &zoom);
	int get_zoom();
	// 絞り設定
	int update_iris_limit(control_value_t &value);
	int set_iris(const int &iris);
	int get_iris();
	// 絞り設定(相対)
	int update_iris_rel_limit(control_value_t &value);
	int set_iris_rel(const int &iris);
	int get_iris_rel();
	// パン・チルト設定
	int update_pan_tilt_limit(control_value2_t &value);
	int set_pan_tilt(const int &pan, const int &tilt);
	uint64_t get_pan_tilt();
	// パン設定
	int set_pan(const int &pan);
	int get_pan();
	// チルト設定
	int set_tilt(const int &tilt);
	int get_tilt();
	// パン・チルト設定(相対)
	int update_pan_tilt_rel_limit(control_value4_t &value);
	int set_pan_tilt_rel(const int &pan, const int &pan_speed, const int &tilt, const int &tilt_speed);
	int get_pan_tilt_rel();
	// パン設定(相対)
	int set_pan_rel(const int &pan_rel, const int &pan_speed);
	int get_pan_rel();
	// チルト設定(相対)
	int set_tilt_rel(const int &tilt_rel, const int &tilt_speed);
	int get_tilt_rel();
	// ロール設定
	int update_roll_limit(control_value_t &value);
	int set_roll(const int &roll);
	int get_roll();
	// ロール設定(相対)
	int update_roll_rel_limit(control_value_t &value);
	int set_roll_rel(const int &roll_rel, const int &roll_speed);
	int get_roll_rel();
	// プライバシーモード
	int update_privacy_limit(control_value_bool_t &value);
	int set_privacy(const int &privacy);
	int get_privacy();
	// オートホワイトバランス設定(コンポ)
	int update_auto_white_blance_compo_limit(control_value_bool_t &value);
	int set_auto_white_blance_compo(const bool &autoWhiteBlanceCompo);
	bool get_auto_white_blance_compo();
	// ホワイトバランス設定(コンポ)
	int update_white_blance_compo_limit(control_value_t &value);
	int set_white_blance_compo(const int &white_blance_compo);
	int get_white_blance_compo();
	// バックライト補正
	int update_backlight_comp_limit(control_value_t &value);
	int set_backlight_comp(const int16_t &backlight);
	int get_backlight_comp();
	// オートコントラスト
	int update_auto_contrast_limit(control_value_bool_t &value);
	int set_auto_contrast(const bool &autoContrast);
	bool get_auto_contrast();
	// オート色相
	int update_auto_hue_limit(control_value_bool_t &value);
	int set_auto_hue(const bool &auto_hue);
	bool get_auto_hue();
	// ズーム(相対)
	int update_zoom_rel_limit(control_value_t &value);
	int set_zoom_rel(const int &zoom);
	int get_zoom_rel();
	// デジタルズーム
	int update_digital_multiplier_limit(control_value_t &value);
	int set_digital_multiplier(const int &multiplier);
	int get_digital_multiplier();
	// デジタルズーム上限設定
	int update_digital_multiplier_limit_limit(control_value_t &value);
	int set_digital_multiplier_limit(const int &multiplier_limit);
	int get_digital_multiplier_limit();
	// アナログビデオ入力規格
	int update_analog_video_standard_limit(control_value_t &value);
	int set_analog_video_standard(const int &standard);
	int get_analog_video_standard();
	// アナログビデオ入力垂直同期設定
	int update_analog_video_lock_state_limit(control_value_t &value);
	int set_analog_video_lock_state(const int &status);
	int get_analog_video_lock_state();
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //USBWEBCAMERAPROJ_UVC_CONTROLLER_H
