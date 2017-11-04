#include "animation.h"

#include "base/assert.h"
#include "base/timer.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std::chrono_literals;

constexpr auto animation_interval = 1.s / 60;

namespace gfx {
namespace impl {

struct animation_controller {
  void start_animation(animation* anim);
  void stop_animation(animation* anim);

  void on_tick();

  base::timer timer_{ [this] { on_tick(); } };
  std::vector<animation*> animations_;
};

void animation_controller::start_animation(animation* anim) {
  if (animations_.empty()) {
    timer_.set(animation_interval, true);
  }

  animations_.push_back(anim);
}

void animation_controller::stop_animation(animation* anim) {
  auto anim_pos = std::find(animations_.begin(), animations_.end(), anim);

  ASSERT(anim_pos != animations_.end()) << "Animation not running";
  animations_.erase(anim_pos);

  if (animations_.empty()) {
    timer_.cancel();
  }
}


void animation_controller::on_tick() {
  auto now = base::task::clock_type::now();

  for (animation* anim : animations_) {
    anim->step(now);
  }
}

}  // namespace impl

namespace {

impl::animation_controller anim_controller;

}  // namespace


animation::animation(easing_func easing, progress_callback callback)
  : callback_(std::move(callback))
  , easing_(std::move(easing)) {
}

animation::~animation() {
  stop();
}


void animation::animate_to(double target_progress) {
  target_progress_ = target_progress;
  initial_progress_ = progress_;

  computed_duration_ = std::abs(progress_ - target_progress_) * duration_;
  start();
}

void animation::jump_to(double progress) {
  stop();
  progress_ = progress;
  callback_(progress_);
}


void animation::enter() {
  animate_to(1.0);
}

void animation::leave() {
  animate_to(0.0);
}

void animation::stop() {
  if (is_running_) {
    anim_controller.stop_animation(this);
    is_running_ = false;
  }
}


// PRIVATE

void animation::start() {
  if (computed_duration_ == 0.0ms) {
    jump_to(target_progress_);
    return;
  }

  start_time_ = base::task::clock_type::now();

  if (!is_running_) {
    anim_controller.start_animation(this);
    is_running_ = true;
  }
}

void animation::step(const base::task::run_time_type& now) {
  auto elapsed_time = now - start_time_;
  double relative_progress = std::min(elapsed_time / computed_duration_, 1.0);
  
  if (relative_progress == 1.0) {
    stop();
  }

  progress_ = initial_progress_ + (target_progress_ - initial_progress_) * easing_(relative_progress);

  callback_(progress_);
}

}  // namespace gfx