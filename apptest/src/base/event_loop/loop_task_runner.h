#pragma once

#include "base/non_copyable.h"
#include "base/task_runner/task_runner.h"
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

namespace base {

class event_loop;

class loop_task_runner : public task_runner {
  friend event_loop;

  loop_task_runner() = default;

public:
  using ptr = std::shared_ptr<loop_task_runner>;

  void quit_now();
  void post_quit();

  static ptr current();

private:
  using task_queue = std::queue<task>;
  using delayed_task_queue = std::priority_queue<task>;

  void do_post_task(task&& tsk) override;

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

  event_loop* current_loop_ = nullptr;
  std::mutex loop_lock_;  // prevents current_loop_ from changing while it is waking up
};

}  // namespace base