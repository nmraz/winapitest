#include "timer.h"

#include "base/event_loop/next_tick.h"
#include <utility>

namespace base {

struct timer::posted_task {
	posted_task(timer* timer)
		: timer_(timer) {}

	void cancel() { timer_ = nullptr; }
	void run();

	timer* timer_;
};

void timer::posted_task::run() {
	if (timer_) {
		timer_->fire();
	}
}


timer::~timer() {
	cancel();
}


void timer::cancel() {
	if (is_running()) {
		current_task_->cancel();
		current_task_ = nullptr;
	}
}

bool timer::is_running() const {
	return current_task_ != nullptr;
}


slot_handle timer::on_fire(callback_type slot) {
	return fire_signal_.connect(std::move(slot));
}


// PRIVATE

void timer::do_set(const task::delay_type& interval, bool repeat) {
	cancel();

	repeating_ = repeat;
	interval_ = interval;
	current_task_ = std::make_shared<posted_task>(this);

	repost_task();
}


void timer::fire() {
	if (repeating_) {
		repost_task();
	}

	fire_signal_();

	if (!repeating_) {
		current_task_ = nullptr;  // no need to cancel() here, as the task will never run again
	}
}


void timer::repost_task() {
	set_timeout([task = current_task_] { task->run(); }, interval_);
}

}  // namespace base