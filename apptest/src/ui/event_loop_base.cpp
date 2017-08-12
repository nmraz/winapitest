#include "event_loop_base.h"

#include <algorithm>
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
		if (msg.hwnd == message_window_.get()) {
			if (msg.message == wake_msg) {
				clear_wake_flag();  // make sure that we can wake up again
			}
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
}

void event_loop_base::wake_up() {
	if (posted_wake_up_.exchange(true, std::memory_order_relaxed)) {  // already woke up
		return;
	}

	::PostMessageW(message_window_.get(), wake_msg, 0, 0);
}


// PRIVATE

LRESULT event_loop_base::handle_message(UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {
	case wake_msg:
		clear_wake_flag();
		run_pending_task();
		break;
		
	case WM_TIMER:
		run_delayed_task();
		break;

	default:
		return 0;
	}

	reschedule_timer();
	return 0;
}

void event_loop_base::clear_wake_flag() {
	posted_wake_up_.store(false, std::memory_order_relaxed);
}

void event_loop_base::reschedule_timer() {
	auto next_run_time = get_next_run_time();

	if (next_run_time != current_next_run_time_) {
		if (next_run_time) {
			auto delay = std::max(*next_run_time - base::task::clock_type::now(), base::task::delay_type::zero());
			::SetTimer(message_window_.get(), 1, get_win_wait_time(delay), nullptr);
		} else {
			::KillTimer(message_window_.get(), 1);
		}

		current_next_run_time_ = next_run_time;
	}
}

}  // namespace ui