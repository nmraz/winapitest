#include "event_loop_base.h"

#include <Windows.h>

namespace ui {
namespace {

constexpr UINT wake_msg = WM_USER;

DWORD get_win_wait_time(const base::task::delay_type& delay) {
	auto millis = std::chrono::ceil<std::chrono::milliseconds>(delay);
	return static_cast<DWORD>(millis.count());
}

}  // namepsace


event_loop_base::event_loop_base()
	: message_window_([this] (UINT msg, WPARAM wparam, LPARAM lparam) {
		return handle_message(msg, wparam, lparam);
	})
	, posted_wake_up_(false) {
}


bool event_loop_base::do_work() {
	reschedule_timer();
	MSG msg;
	if (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
		// we process task_runner tasks directly in this case
		if (msg.hwnd == message_window_.get() && (msg.message == WM_TIMER || msg.message == wake_msg)) {
			return false;
		}
		if (msg.message == WM_QUIT) {
			quit();
			::PostQuitMessage(static_cast<int>(msg.lParam));  // in case we're in a nested loop
			return false;
		}

		process_message(msg);
		return true;
	}
	return false;
}


void event_loop_base::sleep(const std::optional<base::task::delay_type>& delay) {
	DWORD wait_time = INFINITE;
	if (delay) {
		wait_time = get_win_wait_time(*delay);
	}

	::MsgWaitForMultipleObjects(0, nullptr, false, wait_time, QS_ALLINPUT);
	posted_wake_up_.store(false, std::memory_order_relaxed);
}

void event_loop_base::wake_up() {
	if (posted_wake_up_.exchange(true, std::memory_order_relaxed)) {  // already woke up
		return;
	}

	::PostMessageW(message_window_.get(), wake_msg, 0, 0);
}


// PRIVATE

LRESULT event_loop_base::handle_message(UINT msg, WPARAM wparam, LPARAM lparam) {
	posted_wake_up_.store(false, std::memory_order_relaxed);
	bool ran_task;

	switch (msg) {
	case wake_msg:
		ran_task = run_pending_task();
		break;
	case WM_TIMER:
		ran_task = run_delayed_task();
		break;
	default:
		return ::DefWindowProcW(message_window_.get(), msg, wparam, lparam);
	}

	if (ran_task) {
		wake_up();  // give ourselves another chance to run tasks
	}
	reschedule_timer();
	return 0;
}

void event_loop_base::reschedule_timer() {
	auto delay = next_delay();

	if (delay != cached_next_delay_) {
		if (delay) {
			::SetTimer(message_window_.get(), 1, get_win_wait_time(*delay), nullptr);
		} else {
			::KillTimer(message_window_.get(), 1);
		}

		cached_next_delay_ = delay;
	}
}

}  // namespace ui