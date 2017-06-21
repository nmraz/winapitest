#pragma once

#include "base/event_loop/event_loop.h"
#include <condition_variable>
#include <mutex>

namespace base {

class task_event_loop : public event_loop {
public:
	void sleep(const std::optional<task::delay_type>& delay) override;
	void wake_up() override;

private:
	// wake-up mechanism
	bool should_wake_up_;
	std::mutex wake_up_lock_;
	std::condition_variable wake_up_cv_;
};

}