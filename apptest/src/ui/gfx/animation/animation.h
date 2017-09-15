#pragma once

#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"
#include <chrono>
#include <functional>

namespace gfx {

class animation : public base::non_copy_movable {
public:
	using progress_callback = std::function<void(double value)>;
	using easing_func = std::function<double(double prog)>;

	using duration_type = std::chrono::duration<double, std::milli>;


	explicit animation(easing_func easing);
	~animation();

	void set_duration(const duration_type& duration) { duration_ = duration; }
	duration_type duration() const { return duration_; }

	void set_callback(progress_callback callback) { callback_ = std::move(callback); }

	void animate_to(double progress);
	void jump_to(double progress);

	void enter();
	void leave();
	void stop();

	bool is_running() const { return timer_slot_.connected(); }

private:
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

}  // namespace gfx