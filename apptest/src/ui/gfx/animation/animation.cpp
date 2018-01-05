#include "animation.h"

#include "base/assert.h"
#include "base/timer.h"
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std::chrono_literals;

constexpr auto animation_interval = 1.s / 60;

namespace gfx::impl {

struct animation_controller {
  void start_animation(animation_base* anim);
  void stop_animation(animation_base* anim);

  void on_tick();

  base::timer timer_{ [this] { on_tick(); } };
  std::vector<animation_base*> animations_;
};

void animation_controller::start_animation(animation_base* anim) {
  if (animations_.empty()) {
    timer_.set(animation_interval, true);
  }

  animations_.push_back(anim);
}

void animation_controller::stop_animation(animation_base* anim) {
  auto anim_pos = std::find(animations_.begin(), animations_.end(), anim);

  ASSERT(anim_pos != animations_.end()) << "Animation not running";
  animations_.erase(anim_pos);

  if (animations_.empty()) {
    timer_.cancel();
  }
}


void animation_controller::on_tick() {
  auto now = base::task::clock_type::now();

  for (animation_base* anim : animations_) {
    anim->step(now);
  }
}

namespace {

impl::animation_controller anim_controller;

}  // namespace


animation_base::animation_base(easing_func easing)
  : easing_(std::move(easing)) {
}

animation_base::~animation_base() {
  stop();
}


// PROTECTED

void animation_base::start() {
  if (duration_ == 0.0ms) {
    stop();
    do_step(1.0);
    return;
  }

  start_time_ = base::task::clock_type::now();

  if (!is_running_) {
    anim_controller.start_animation(this);
    is_running_ = true;
  }
}

void animation_base::stop() {
  if (is_running_) {
    anim_controller.stop_animation(this);
    is_running_ = false;
  }
}


// PRIVATE

void animation_base::step(base::task::run_time_type now) {
  auto elapsed_time = now - start_time_;
  double prog = std::min(elapsed_time / duration_, 1.0);

  if (prog == 1.0) {
    stop();
  }

  do_step(prog);
}

}  // namespace gfx::impl