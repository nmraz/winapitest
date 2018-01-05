#pragma once

#include "base/function.h"
#include "base/non_copyable.h"
#include "base/task_runner/task.h"
#include "ui/gfx/util.h"
#include <chrono>

namespace gfx {
namespace impl {

struct animation_controller;

class animation_base : public base::non_copy_movable {
public:
  using easing_func = base::function<double(double)>;
  using duration_type = std::chrono::duration<double, std::milli>;

  explicit animation_base(easing_func easing);
  ~animation_base();

  void set_duration(duration_type duration) { duration_ = duration; }
  duration_type duration() const { return duration_; }

  bool is_running() const { return is_running_; }

protected:
  void start();
  void stop();

  virtual void do_step(double prog) = 0;

private:
  friend animation_controller;

  void step(base::task::run_time_type now);

  easing_func easing_;
  bool is_running_ = false;

  duration_type duration_;
  base::task::run_time_type start_time_;
};

}  // namespace impl


template<typename T>
struct default_anim_traits {
  static T lerp(const T& from, const T& to, double t) {
    return lerp(from, to, t);  // ADL
  }
};


template<typename T, typename Traits = default_anim_traits<T>>
class animation : public impl::animation_base {
public:
  using progress_callback = base::function<void(animation&)>;

  explicit animation(easing_func easing, progress_callback callback);

  void set(const T& val);
  void animate_to(const T& val);

  T val() const { return val_; }

private:
  void do_step(double prog) override;
  void update(const T& val);

  progress_callback callback_;

  T val_{};
  T initial_val_;
  T target_val_;
};


template<typename T, typename Traits>
animation<T, Traits>::animation(easing_func easing, progress_callback callback)
  : impl::animation_base(std::move(easing))
  , callback_(std::move(callback)) {
}

template<typename T, typename Traits>
void animation<T, Traits>::set(const T& val) {
  stop();
  update(val);
}

template<typename T, typename Traits>
void animation<T, Traits>::animate_to(const T& val) {
  target_val_ = val;
  initial_val_ = val_;
  start();
}


// PRIVATE

template<typename T, typename Traits>
void animation<T, Traits>::do_step(double prog) {
  update(Traits::lerp(initial_val_, target_val_, prog));
}

template<typename T, typename Traits>
void animation<T, Traits>::update(const T& val) {
  val_ = val;
  callback_(*this);
}

}  // namespace gfx