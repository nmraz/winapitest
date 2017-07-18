#include "animation.h"

#include "base/timer.h"
#include <algorithm>
#include <cmath>

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


animation::animation(easing_func easing)
	: easing_(std::move(easing)) {
}

animation::~animation() {
	stop();
}


void animation::animate_to(double target_progress) {
	target_progress_ = std::min(std::max(target_progress, 0.0), 1.0);  // until we have std::clamp
	initial_progress_ = progress_;
	computed_duration_ = std::abs(progress_ - target_progress_) * duration_;
	start();
}

void animation::jump_to(double progress) {
	stop();
	progress_ = progress;
	callback_(easing_(progress_), true);
}


void animation::enter() {
	animate_to(1.0);
}

void animation::leave() {
	animate_to(0.0);
}

void animation::stop() {
	remove_timer_listener(timer_slot_);
}


// PRIVATE

void animation::start() {
	if (computed_duration_ == 0.0ms) {
		stop();
		return;
	}
	start_time_ = std::chrono::steady_clock::now();
	if (!is_running()) {
		timer_slot_ = add_timer_listener([this] { on_progress(); });
	}
}

void animation::on_progress() {
	auto elapsed_time = std::chrono::steady_clock::now() - start_time_;
	double relative_progress = elapsed_time / computed_duration_;
	if (relative_progress >= 1.0) {
		relative_progress = 1.0;
		stop();
	}
	progress_ = initial_progress_ + (target_progress_ - initial_progress_) * relative_progress;
	callback_(easing_(progress_), relative_progress == 1.0);
}


}  // namespace gfx