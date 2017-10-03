#include "next_tick.h"

#include "base/event_loop/loop_task_runner.h"

namespace base::impl {

void do_set_timeout(task::callback_type&& callback, const task::delay_type& delay) {
  loop_task_runner::current()->post_task(std::move(callback), delay);
}

}  // namespace base::impl