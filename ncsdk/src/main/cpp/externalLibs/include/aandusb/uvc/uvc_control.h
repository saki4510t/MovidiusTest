/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef UVC_CONTROL_H_
#define UVC_CONTROL_H_

// uvc
#include "aanduvc.h"

namespace serenegiant {
namespace usb {

class Device;

namespace uvc {

class VideoControlInterface;

/**
*　実際のUVCコントロールの処理をするクラス
*/
class UVCControl {
private:
	Device *device;
	VideoControlInterface *interface;
	int bInterfaceNumber;
	const processing_unit_descriptor_t *processing_unit_desc;
	int wIndexProcessingUnit;
	const input_terminal_descriptor_t *input_terminal_desc;
	int wIndexInputTerminal;
protected:
public:
	UVCControl(VideoControlInterface *interface);
	virtual ~UVCControl();
	int get_ctrl_len(uint8_t unit, uint8_t ctrl);
	int get_ctrl(uint8_t unit, uint8_t ctrl, uint8_t *data, int len, req_code_t req_code);
	int set_ctrl(uint8_t unit, uint8_t ctrl, uint8_t *data, int len);
	// ERROR CONTROLS
	int get_vc_error_code(vc_error_code_control_t &error_code, req_code_t req_code);
	inline vc_error_code_control_t get_vc_error_code() {
		vc_error_code_control_t error_code;
		if (!(vc_error_code_control_t)get_vc_error_code(error_code, REQ_GET_CUR))
			return error_code;
		else
			return VC_ERROR_CODECTRL_ERROR;
	}

	// INTERFACE CONTROLS
	int get_power_mode(device_power_mode_t &mode, req_code_t req_code);
	int set_power_mode(device_power_mode_t mode);
	// CAMERA TERMINAL CONTROLS
	int get_ae_mode(int &mode, req_code_t req_code);
	int set_ae_mode(int mode);
	int get_ae_priority(uint8_t &priority, req_code_t req_code);
	int set_ae_priority(uint8_t priority);
	int get_exposure_abs(int &time, req_code_t req_code);
	int set_exposure_abs(int time);
	int get_exposure_rel(int &step, req_code_t req_code);
	int set_exposure_rel(int step);
	int get_scanning_mode(uint8_t &mode, req_code_t req_code);
	int set_scanning_mode(uint8_t mode);
	int get_focus_auto(uint8_t &autofocus, req_code_t req_code);
	int set_focus_auto(uint8_t autofocus);
	int get_focus_abs(short &focus, req_code_t req_code);
	int set_focus_abs(short focus);
	int get_focus_rel(int8_t &focus, uint8_t &speed, req_code_t req_code);
	int set_focus_rel(int8_t focus, uint8_t speed);
	int get_iris_abs(uint16_t &iris, req_code_t req_code);
	int set_iris_abs(uint16_t iris);
	int get_iris_rel(uint8_t &iris, req_code_t req_code);
	int set_iris_rel(uint8_t iris);
	int get_zoom_abs(uint16_t &zoom, req_code_t req_code);
	int set_zoom_abs(uint16_t zoom);
	int get_zoom_rel(int8_t &zoom, uint8_t &isdigital, uint8_t &speed, req_code_t req_code);
	int set_zoom_rel(int8_t zoom, uint8_t isdigital, uint8_t speed);
	int get_pantilt_abs(int32_t &pan, int32_t &tilt, req_code_t req_code);
	int set_pantilt_abs(int32_t pan, int32_t tilt);
	int get_pantilt_rel(int8_t &pan_rel, uint8_t &pan_speed,
			int8_t &tilt_rel, uint8_t &tilt_speed,
			req_code_t req_code);
	int set_pantilt_rel(int8_t pan_rel, uint8_t pan_speed,
			int8_t tilt_rel, uint8_t tilt_speed);
	int get_roll_abs(int16_t &roll, req_code_t req_code);
	int set_roll_abs(int16_t roll);
	int get_roll_rel(int8_t &roll_rel, uint8_t &speed, req_code_t req_code);
	int set_roll_rel(int8_t roll_rel, uint8_t speed);
	int get_privacy(uint8_t &privacy, req_code_t req_code);
	int set_privacy(uint8_t privacy);
	int get_digital_window(uint16_t &window_top, uint16_t &window_left,
			uint16_t &window_bottom, uint16_t &window_right,
			uint16_t &num_steps, uint16_t &num_steps_units,
			req_code_t req_code);
	int set_digital_window(uint16_t window_top, uint16_t window_left,
			uint16_t window_bottom, uint16_t window_right,
			uint16_t num_steps, uint16_t num_steps_units);
	int get_digital_roi(uint16_t &roi_top, uint16_t &roi_left,
			uint16_t &roi_bottom, uint16_t &roi_right, uint16_t &auto_controls,
			req_code_t req_code);
	int set_digital_roi(uint16_t roi_top, uint16_t roi_left,
			uint16_t roi_bottom, uint16_t roi_right, uint16_t auto_controls);
	// SELECTOR UNIT CONTROLS
	// PROCESSING UNIT CONTROLS
	int get_backlight_compensation(short &comp, req_code_t req_code);
	int set_backlight_compensation(short comp);
	int get_brightness(short &brightness, req_code_t req_code);
	int set_brightness(short brightness);
	int get_contrast(uint16_t &contrast, req_code_t req_code);
	int set_contrast(uint16_t contrast);
	int get_contrast_auto(uint8_t &autoContrast, req_code_t req_code);
	int set_contrast_auto(uint8_t autoContrast);
	int get_gain(uint16_t &gain, req_code_t req_code);
	int set_gain(uint16_t gain);
	int get_powerline_freqency(uint8_t &freq, req_code_t req_code);
	int set_powerline_freqency(uint8_t freq);
	int get_hue(short &hue, req_code_t req_code);
	int set_hue(short hue);
	int get_hue_auto(uint8_t &autoHue, req_code_t req_code);
	int set_hue_auto(uint8_t autoHue);
	int get_saturation(uint16_t &saturation, req_code_t req_code);
	int set_saturation(uint16_t saturation);
	int get_sharpness(uint16_t &sharpness, req_code_t req_code);
	int set_sharpness(uint16_t sharpness);
	int get_gamma(uint16_t &gamma, req_code_t req_code);
	int set_gamma(uint16_t gamma);
	int get_white_balance_temperature(uint16_t &wb_temperature, req_code_t req_code);
	int set_white_balance_temperature(uint16_t wb_temperature);
	int get_white_balance_temperature_auto(uint8_t &autoWbTemp, req_code_t req_code);
	int set_white_balance_temperature_auto(uint8_t autoWbTemp);
	int get_white_balance_component(uint32_t &wb_compo, req_code_t req_code);
	int set_white_balance_component(uint32_t wb_compo);
	int get_white_balance_component_auto(uint8_t &autoWbCompo, req_code_t req_code);
	int set_white_balance_component_auto(uint8_t autoWbCompo);
	int get_digital_multiplier(uint16_t &multiplier, req_code_t req_code);
	int set_digital_multiplier(uint16_t multiplier);
	int get_digital_multiplier_limit(uint16_t &limit, req_code_t req_code);
	int set_digital_multiplier_limit(uint16_t limit);
	int get_analog_video_standard(uint8_t &standard, req_code_t req_code);
	int set_analog_video_standard(uint8_t standard);
	int get_analog_video_lockstate(uint8_t &lock_state, req_code_t req_code);
	int set_analog_video_lockstate(uint8_t lock_state);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant
#endif /* UVC_CONTROL_H_ */
