#pragma once

#include "base/function.h"
#include <chrono>

namespace base {

struct task {
  using callback_type = function<void()>;

  using clock_type = std::chrono::steady_clock;
  using run_time_type = clock_type::time_point;
  using delay_type = run_time_type::duration;

  task(callback_type callback, const run_time_type& run_time);
  bool operator<(const task& rhs) const;

  // ensure that std::terminate is called if the callback throws
  void run() noexcept {
    callback();
  }

  callback_type callback;
  run_time_type run_time;
};

}  // namespace base
