#include "loop_task_runner.h"

#include "base/assert.h"
#include "base/event_loop/event_loop.h"
#include <utility>

namespace base {
namespace {

thread_local loop_task_runner::ptr current_runner;

}  // namespace

void loop_task_runner::post_quit() {
  post_task([this] { quit_now(); });
}

void loop_task_runner::quit_now() {
  if (event_loop* cur_loop = event_loop::current()) {
    if (event_loop::nesting_level() > 1) {
      post_quit();
    }
    cur_loop->quit();
  }
}


// static
void loop_task_runner::init_for_this_thread() {
  current_runner = ptr(new loop_task_runner);
}

// static
loop_task_runner::ptr loop_task_runner::current() {
  ASSERT(current_runner) << "Loop task runner not initialized on this thread";
  return current_runner;
}


// PRIVATE

void loop_task_runner::do_post_task(task&& tsk) {
  bool was_empty;
  {
    std::lock_guard hold(task_lock_);

    was_empty = task_queue_.empty();
    task_queue_.push(std::move(tsk));
  }

  if (was_empty) {
    std::lock_guard hold_loop(loop_lock_);  // current_loop_ mustn't change until after wake_up
    if (current_loop_) {
      current_loop_->wake_up();
    }
  }
}


bool loop_task_runner::run_pending_task() {
  while (true) {
    swap_queues();
    if (current_tasks_.empty()) {
      break;  // no more tasks, not even in the incoming queue
    }

    while (!current_tasks_.empty()) {
      task current_task = std::move(current_tasks_.front());
      current_tasks_.pop();

      if (current_task.run_time == task::run_time_type()) {
        current_task.run();
        return true;
      }
      delayed_tasks_.push(std::move(current_task));
    }
  }

  return false;
}

bool loop_task_runner::run_delayed_task() {
  if (delayed_tasks_.empty()) {
    return false;
  }

  const task& current_task = delayed_tasks_.top();

  if (current_task.run_time > cached_now_) {
    if (current_task.run_time > (cached_now_ = task::clock_type::now())) {
      return false;
    }
  }

  task tmp_task = std::move(const_cast<task&>(current_task));  // current_task will still be at the top
  delayed_tasks_.pop();  // the task must be dequeued *before* running
  tmp_task.run();

  return true;
}


std::optional<task::run_time_type> loop_task_runner::get_next_run_time() const {
  if (delayed_tasks_.empty()) {
    return std::nullopt;
  }

  return delayed_tasks_.top().run_time;
}


void loop_task_runner::swap_queues() {
  if (current_tasks_.empty()) {
    std::lock_guard hold(task_lock_);
    task_queue_.swap(current_tasks_);
  }
}


void loop_task_runner::set_loop(event_loop* loop) {
  std::lock_guard hold(loop_lock_);
  current_loop_ = loop;
}

}  // namespace base