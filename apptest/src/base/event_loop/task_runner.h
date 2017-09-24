#pragma once

#include "base/non_copyable.h"
#include "base/event_loop/task.h"
#include "base/event_loop/task_runner_handle.h"
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

namespace base {

class event_loop;

namespace impl {
struct task_runner_ref;
}


class task_runner : public non_copy_movable {
  friend event_loop;

  task_runner();
  ~task_runner();

public:
  void post_task(task::callback_type callback, const task::delay_type& delay = task::delay_type::zero());
  void quit_now();
  void post_quit();

  task_runner_handle handle();

  static task_runner& current();

private:
  using task_queue = std::queue<task>;
  using delayed_task_queue = std::priority_queue<task>;

  bool run_pending_task();
  bool run_delayed_task();
  std::optional<task::run_time_type> get_next_run_time() const;

  void swap_queues();

  void set_loop(event_loop* loop);

  task_queue task_queue_;
  std::mutex task_lock_;  // protects task_queue_

  task_queue current_tasks_;  // to avoid locking the mutex every time, process tasks in batches
  delayed_task_queue delayed_tasks_;

  task::run_time_type cached_now_;  // make running more efficient when multiple tasks have to run now

  event_loop* current_loop_;
  std::mutex loop_lock_;  // prevents current_loop_ from changing while it is waking up

  std::shared_ptr<impl::task_runner_ref> handle_ref_;  // synchronizes with handles on other threads
};

}  // namespace base