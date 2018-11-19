//
// Created by saki on 2018/05/03.
//

#ifndef UVC_STATUS_EVENT_HANDLER_H
#define UVC_STATUS_EVENT_HANDLER_H

namespace serenegiant {
namespace usb {
namespace uvc {

class StatusTransfer;

class UVCStatusEventHandler {
private:
	mutable Mutex callback_lock;

	jmethodID mStatusCallbackMethod;
	jmethodID mButtonCallbackMethod;
	jobject mStatusCallbackObj;
	jobject mButtonCallbackObj;

	/**
	 * StatusTransferのエントリーポイント
 	 */
	static void uvc_status_callback(enum uvc_status_class status_class,
		int event, int selector, enum uvc_status_attribute status_attribute,
		const uint8_t *data, size_t data_len, void *user_ptr);
	/**
	 * StatusTransferからのイベント実際の処理
	 */
	void handle_event(const enum uvc_status_class status_class,
		const int event, const int selector,
		const enum uvc_status_attribute status_attribute,
		const uint8_t *data, const size_t &data_len);

protected:
	/**
	 * ステータス受信イベントの処理
	 */
	void handle_event_status(const enum uvc_status_class status_class,
		const int event, const int selector,
		const enum uvc_status_attribute status_attribute,
		const uint8_t *data, const size_t &data_len);

	/**
	 * ハードウエアトリガーイベントの処理
	 */
	void handle_event_hardware_trigger(const int &what, const bool &pressed);
public:
	UVCStatusEventHandler();
	/**
	 * デストラクタ
	 * This class instance should released after all related StatusTransfer instance(s)
	 * otherwise app will crash
	 */
	virtual ~UVCStatusEventHandler();

	void register_transfer(StatusTransfer *status_transfer = NULL);
	/**
	 * ステータスコールバックを登録
	 * @param env
	 * @param Java側のIStatusCallbackインターフェースを実装したコールバックオブジェクト, グローバル参照
	 */
	int set_status_callback(JNIEnv *env, jobject status_callback_obj);

	/**
	 * ボタンコールバック(ハードウエアトリガーコールバック)を登録
	 * @param env
	 * @param Java側のIButtonCallbackインターフェースを実装したコールバックオブジェクト, グローバル参照
	 */
	int set_button_callback(JNIEnv *env, jobject button_callback_obj);
};

}	// end of namespace uvc
}	// end of namespace usb
}	// end of namespace serenegiant

#endif //UVC_STATUS_EVENT_HANDLER_H
