#include "event_loop.h"

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


event_loop::event_loop()
	: message_window_([this] (UINT msg, WPARAM, LPARAM) {
		return handle_message(msg);
	})
	, posted_wake_up_(false) {
}


bool event_loop::do_work() {
	reschedule_timer();

	MSG msg;
	if (!::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
		return false;
	}

	// we process task_runner tasks directly in this case
	if (msg.hwnd == message_window_.get()) {
		bool was_wake_msg = msg.message == wake_msg;

		// avoid starving normal window messages
		bool has_other_message = ::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE);

		if (was_wake_msg) {
			clear_wake_flag();  // make sure that we can wake up again
		}

		if (!has_other_message) {
			return false;
		}
	}

	if (msg.message == WM_QUIT) {
		quit();
		::PostQuitMessage(static_cast<int>(msg.lParam));  // in case we're in a nested loop
		return false;
	}

	wake_up();  // in case we enter a nested loop here

	::TranslateMessage(&msg);
	::DispatchMessageW(&msg);

	return true;
}


void event_loop::sleep(const base::task::delay_type* delay) {
	DWORD wait_time = INFINITE;
	if (delay) {
		wait_time = get_win_wait_time(*delay);
	}

	::MsgWaitForMultipleObjects(0, nullptr, false, wait_time, QS_ALLINPUT);
}

void event_loop::wake_up() {
	if (posted_wake_up_.exchange(true, std::memory_order_relaxed)) {  // already woke up
		return;
	}

	::PostMessageW(message_window_.get(), wake_msg, 0, 0);
}


// PRIVATE

LRESULT event_loop::handle_message(UINT msg) {
	if (msg == wake_msg) {
		// this must *always* be cleared, even if we aren't doing anything else
		clear_wake_flag();
	}

	if (!is_current()) {
		return 0;  // let the current loop do the processing
	}


	switch (msg) {
	case wake_msg:
		if (run_pending_task()) {
			// there may be more pending tasks, so make sure to check
			wake_up();
		}
		run_delayed_task();
		break;
		
	case WM_TIMER:
		run_delayed_task();
		// in case the WM_TIMER was early, we must force reschedule_timer to reschedule
		current_next_run_time_ = std::nullopt;
		break;
	}

	reschedule_timer();
	return 0;
}


void event_loop::clear_wake_flag() {
	posted_wake_up_.store(false, std::memory_order_relaxed);
}


void event_loop::reschedule_timer() {
	auto next_run_time = get_next_run_time();

	if (next_run_time == current_next_run_time_) {
		return;  // nothing has changed
	}

	if (next_run_time) {
		auto delay = *next_run_time - base::task::clock_type::now();
		
		if (delay > base::task::delay_type::zero()) {
			::SetTimer(message_window_.get(), 1, get_win_wait_time(delay), nullptr);
		} else {
			wake_up();  // task overdue, run now
		}

	} else {
		::KillTimer(message_window_.get(), 1);
	}

	current_next_run_time_ = next_run_time;
}

}  // namespace ui