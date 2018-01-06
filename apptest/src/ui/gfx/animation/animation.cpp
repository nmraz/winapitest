#include "animation.h"

#include "base/assert.h"
#include "base/auto_restore.h"
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

  int animation_count_ = 0;
  bool in_tick_ = false;
};

void animation_controller::start_animation(animation_base* anim) {
  animations_.push_back(anim);

  if (!animation_count_++) {
    timer_.set(animation_interval, true);
  }
}

void animation_controller::stop_animation(animation_base* anim) {
  auto anim_pos = std::find(animations_.begin(), animations_.end(), anim);

  ASSERT(anim_pos != animations_.end()) << "Animation not running";
  if (in_tick_) {
    *anim_pos = nullptr;
  } else {
    animations_.erase(anim_pos);
  }

  if (!--animation_count_) {
    timer_.cancel();
  }
}


void animation_controller::on_tick() {
  {
    base::auto_restore<bool> hold_in_tick(in_tick_, true);
    auto now = base::task::clock_type::now();

    for (animation_base* anim : animations_) {
      if (anim) {
        anim->step(now);
      }
    }
  }

  // clean up animations which stopped during this tick
  animations_.erase(std::remove(animations_.begin(), animations_.end(), nullptr), animations_.end());
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