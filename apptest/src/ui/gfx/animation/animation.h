#pragma once

#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"

#include <chrono>
#include <functional>

namespace gfx {

class animation : public base::non_copy_movable {
public:
	using progress_callback = std::function<void(double)>;
	using easing_func = std::function<double(double)>;
	enum class run_state { 
		not_running, 
		entering, 
		leaving
	};

	animation(progress_callback, easing_func);

	template<typename Rep, typename Period>
	void set_duration(const std::chrono::duration<Rep, Period>&);

	void enter();
	void leave();
	void stop();

	run_state get_run_state() const { return state_; }

private:
	void on_progress();
	void start(run_state state);
	
	progress_callback callback_;
	easing_func easing_;
	run_state state_;
	base::slot_handle timer_slot_;

	std::chrono::duration<double, std::milli> duration_;
	std::chrono::steady_clock::time_point start_time_;
};

template<typename Rep, typename Period>
void animation::set_duration(const std::chrono::duration<Rep, Period>& duration) {
	duration_ = duration;
}

}  // namespace gfx