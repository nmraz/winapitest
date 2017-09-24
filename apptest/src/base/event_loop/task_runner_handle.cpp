#include "task_runner_handle.h"

#include "base/event_loop/task_runner.h"
#include "base/event_loop/task_runner_ref.h"
#include <utility>

namespace base {

bad_task_runner_handle::bad_task_runner_handle()
  : std::runtime_error("The requested task_runner is no longer alive") {
}


void task_runner_handle::post_task(task::callback_type callback) {
  if (!ref_) {
    throw bad_task_runner_handle();
  }

  std::shared_lock<std::shared_mutex> hold(ref_->lock);
  task_runner* runner = ref_->runner;
  
  if (!runner) {
    throw bad_task_runner_handle();
  }

  runner->post_task(std::move(callback));
}


// PRIVATE

task_runner_handle::task_runner_handle(std::shared_ptr<impl::task_runner_ref> ref)
  : ref_(std::move(ref)) {
}

// static
task_runner_handle task_runner_handle::current_hande() {
  return task_runner::current().handle();
}

}  // namespace base