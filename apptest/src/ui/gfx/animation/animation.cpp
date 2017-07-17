#include "animation.h"

#include "base/timer.h"

using namespace std::chrono_literals;

namespace gfx {
namespace {

constexpr auto animation_interval = 16ms;
base::timer animation_timer;
int animation_count;

base::slot_handle add_timer_listener(base::timer::callback_type callback) {
	if (!animation_count++) {
		animation_timer.set(animation_interval, true);
	}
	return animation_timer.on_fire(std::move(callback));
}

void remove_timer_listener(base::slot_handle callback_handle) {
	callback_handle.disconnect();
	animation_count--;
	if (!animation_count) {
		animation_timer.cancel();
	}
}
}  // namespace


animation::animation(progress_callback callback, easing_func easing)
	: callback_(std::move(callback))
	, easing_(std::move(easing))
	, state_(run_state::not_running) {
}


void animation::enter() {
	start(run_state::entering);
}

void animation::leave() {
	start(run_state::leaving);
}

void animation::start(run_state state) {
	if (state_ == run_state::not_running) {
		start_time_ = std::chrono::steady_clock::now();
		timer_slot_ = add_timer_listener([this] { on_progress(); });
	}
	state_ = state;
}


void animation::on_progress() {
	auto elapsed_time = std::chrono::steady_clock::now() - start_time_;
	double progress = elapsed_time / duration_;
	if (progress >= 1.0) {
		progress = 1.0;
		stop();
	}
	if (state_ == run_state::leaving) {
		progress = 1 - progress;
	}
	callback_(easing_(progress));
}

void animation::stop() {
	remove_timer_listener(timer_slot_);
}

}  // namespace gfx