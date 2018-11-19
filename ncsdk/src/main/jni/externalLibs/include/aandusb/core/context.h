/*
 * aAndUsb
 * Copyright (c) 2014-2018 saki t_saki@serenegiant.com
 * Distributed under the terms of the GNU Lesser General Public License (LGPL v3.0) License.
 * License details are in the file license.txt, distributed as part of this software.
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <poll.h>
// core
#include "aandusb.h"
#include "internal.h"

namespace serenegiant {
namespace usb {

typedef struct pollfd pollfd_t;

class Device;
class Transfer;

/**
*
*/
class Context {
private:
	// イベント処理コントロール用のセルフパイプ
	int ctrl_pipe[2];	// 0:読み出しパイプ, 1:書き込みパイプ
	std::vector<pollfd_t *> pollfds;
	mutable Mutex pollfds_lock;
	// イベント処理中スレッドカウンタ, 0でなければイベント処理中のスレッドが存在する
	uint32_t pollfd_modify;
	mutable Mutex pollfd_modify_lock;

	std::vector<Device *> devices;
	mutable Mutex devices_lock;

	std::list<Transfer *> transfers;
	mutable Mutex transfers_lock;

	mutable Mutex events_lock;
	mutable Mutex event_waiters_lock;
	Condition event_waiters_cond;

	bool event_handler_active;	// イベント処理中
	bool running;	// イベントハンドリングスレッドの処理中フラグ、落とすとスレッド終了
	// イベントハンドリングスレッド
	pthread_t event_handler_thread;

	int init(int pipefd[2]);
	int add_pollfd(int fd, short events);
	void remove_pollfd(int fd);
	void break_poll();

	static void *event_handler_func(void *args);
	int lock_events();
	bool try_lock_events();
	void unlock_events();
	void event_loop();
	int handle_events(struct timeval &tv, int *completed);
	int poll_events(struct timeval &tv);
	int handle_events_device(pollfd_t *fds, const nfds_t &nfds, int num_ready, const int err);
	bool get_next_timeout(struct timeval &tv, struct timeval &out);
	bool is_event_handler_active();
	int wait_event(struct timeval *tv);
	int check_timeouts();
protected:
public:
	Context();
	virtual ~Context();
	int register_device(Device *device);
	void unregister_device(Device *device, const bool autolock = true);
	int add_transfer(Transfer *transfer, const bool autolock = false);
	int remove_transfer(Transfer *transfer, const bool autolock = false);
	status_t lock_transfer();
	void unlock_transfer();
	void signal_event();
	void release();
	void notify();
};

}	// end of namespace usb
}	// end of namespace serenegiant
#endif /* CONTEXT_H_ */
