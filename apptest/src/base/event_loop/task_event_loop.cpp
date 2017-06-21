#include "task_event_loop.h"

namespace base {

void task_event_loop::sleep(const std::optional<task::delay_type>& delay) {
	std::unique_lock<std::mutex> hold(wake_up_lock_);

	if (delay) {
		wake_up_cv_.wait_for(hold, *delay, [this] { return should_wake_up_; });
	} else {
		wake_up_cv_.wait(hold, [this] { return should_wake_up_; });
	}
	should_wake_up_ = false;
}

void task_event_loop::wake_up() {
	std::lock_guard<std::mutex> hold(wake_up_lock_);
	should_wake_up_ = true;
	wake_up_cv_.notify_one();
}

}  // namespace base