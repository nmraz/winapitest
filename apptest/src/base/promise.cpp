#include "promise.h"

#include "base/event_loop/loop_task_runner.h"

namespace base {

bad_promise_val::bad_promise_val()
  : std::logic_error("Bad promise value") {
}

abandoned_promise::abandoned_promise()
  : std::logic_error("Abandoned promise") {
}

namespace impl {

std::shared_ptr<task_runner> default_promise_task_runner() {
  return loop_task_runner::current();
}

}  // namespace impl
}  // namespace base