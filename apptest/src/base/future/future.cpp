#include "future.h"

#include "base/event_loop/loop_task_runner.h"

namespace base::impl {

std::shared_ptr<task_runner> default_then_task_runner() {
  return loop_task_runner::current();
}

}  // namespace base::impl