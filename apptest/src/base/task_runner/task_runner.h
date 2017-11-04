#pragma once

#include "base/non_copyable.h"
#include "base/task_runner/cancellation.h"
#include "base/task_runner/task.h"

namespace base {

class task_runner : public non_copy_movable {
public:
  virtual ~task_runner() {}

  void post_task(task::callback_type callback);
  template<typename Cb>
  void post_task(cancellation_token tok, Cb&& callback);

  void post_task(task::callback_type callback, const task::delay_type& delay);
  template<typename Cb>
  void post_task(cancellation_token tok, Cb&& callback, const task::delay_type& delay);

  void post_task(task::callback_type callback, const task::run_time_type& run_time);
  template<typename Cb>
  void post_task(cancellation_token tok, Cb&& callback, const task::run_time_type& run_time);

protected:
  virtual void do_post_task(task&& tsk) = 0;
};


template<typename Cb>
void task_runner::post_task(cancellation_token tok, Cb&& callback) {
  post_task([callback = std::forward<Cb>(callback), tok = std::move(tok)] {
    if (!tok.is_canceled()) {
      callback();
    }
  });
}

template<typename Cb>
void task_runner::post_task(cancellation_token tok, Cb&& callback, const task::delay_type& delay) {
  post_task([callback = std::forward<Cb>(callback), tok = std::move(tok)] {
    if (!tok.is_canceled()) {
      callback();
    }
  }, delay);
}

template<typename Cb>
void task_runner::post_task(cancellation_token tok, Cb&& callback, const task::run_time_type& run_time) {
  post_task([callback = std::forward<Cb>(callback), tok = std::move(tok)] {
    if (!tok.is_canceled()) {
      callback();
    }
  }, run_time);
}

}  // namespace base
