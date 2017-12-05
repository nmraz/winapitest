#pragma once

#include "base/function.h"
#include "base/non_copyable.h"
#include "base/task_runner/task.h"
#include <chrono>

namespace gfx {
namespace impl {

struct animation_controller;

}  // namespace impl


class animation : public base::non_copy_movable {
public:
  using progress_callback = base::function<void(double)>;
  using easing_func = base::function<double(double)>;

  using duration_type = std::chrono::duration<double, std::milli>;


  explicit animation(easing_func easing, progress_callback callback = nullptr);
  ~animation();

  void set_duration(const duration_type& duration) { duration_ = duration; }
  duration_type duration() const { return duration_; }

  void set_callback(progress_callback callback) { callback_ = std::move(callback); }

  void animate_to(double progress);
  void set(double progress);

  void enter();
  void leave();

  void stop();
  void reset() { set(0.0); }

  bool is_running() const { return is_running_; }

private:
  friend impl::animation_controller;

  void start();
  void step(const base::task::run_time_type& now);

  progress_callback callback_;
  easing_func easing_;

  bool is_running_ = false;

  double progress_ = 0.0;
  double initial_progress_;
  double target_progress_;  // used by animate_to

  duration_type duration_;
  duration_type computed_duration_;  // used by animate_to
  base::task::run_time_type start_time_;
};

}  // namespace gfx