#pragma once

#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"

#include <chrono>
#include <functional>

namespace gfx {

class animation : public base::non_copy_movable {
public:
	using progress_callback = std::function<void(double, bool)>;
	using easing_func = std::function<double(double)>;

	animation(progress_callback, easing_func);

	template<typename Rep, typename Period>
	void set_duration(const std::chrono::duration<Rep, Period>&);

	void animate_to(double progress);
	void enter();
	void leave();
	void stop();

	bool is_running() const { return timer_slot_.connected(); }

private:
	using duration_type = std::chrono::duration<double, std::milli>;

	void start();
	void on_progress();

	progress_callback callback_;
	easing_func easing_;

	base::slot_handle timer_slot_;

	double progress_ = 0.0;
	double initial_progress_;
	double target_progress_;  // used by animate_to

	duration_type duration_;
	duration_type computed_duration_;  // used by animate_to
	std::chrono::steady_clock::time_point start_time_;
};

template<typename Rep, typename Period>
void animation::set_duration(const std::chrono::duration<Rep, Period>& duration) {
	duration_ = duration;
}

}  // namespace gfx