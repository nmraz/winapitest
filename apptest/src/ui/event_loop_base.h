#pragma once

#include "base/event_loop/event_loop.h"
#include "base/win/message_window.h"
#include <atomic>

namespace ui {

class event_loop_base : public base::event_loop {
public:
	event_loop_base();
	bool do_work() override final;
	void sleep(const std::optional<base::task::delay_type>& delay) override final;
	void wake_up() override final;

	virtual void process_message(const MSG& msg) = 0;

private:
	LRESULT handle_message(UINT msg, WPARAM wparam, LPARAM lparam);
	void reschedule_timer();

	base::win::message_window message_window_;
	std::atomic<bool> posted_wake_up_;
	std::optional<base::task::delay_type> cached_next_delay_;
};

}  // namespace ui