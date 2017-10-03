#include "task_runner.h"

namespace base {

void task_runner::post_task(task::callback_type callback) {
  do_post_task(task(std::move(callback), task::run_time_type()));
}

void task_runner::post_task(task::callback_type callback, const task::delay_type& delay) {
  auto run_time = delay > task::delay_type::zero() ?
    task::clock_type::now() + delay : task::run_time_type();
  do_post_task(task(std::move(callback), run_time));
}

void task_runner::post_task(task::callback_type callback, const task::run_time_type& run_time) {
  auto computed_run_time = run_time <= task::clock_type::now() ? task::run_time_type() : run_time;
  do_post_task(task(std::move(callback), computed_run_time));
}

}  // namespace base