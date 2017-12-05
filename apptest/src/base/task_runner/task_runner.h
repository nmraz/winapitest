#pragma once

#include "base/non_copyable.h"
#include "base/task_runner/task.h"

namespace base {

class task_runner : public non_copy_movable {
public:
  virtual ~task_runner() {}

  void post_task(task::callback_type callback);
  void post_task(task::callback_type callback, task::delay_type delay);
  void post_task(task::callback_type callback, task::run_time_type run_time);

protected:
  virtual void do_post_task(task&& tsk) = 0;
};

}  // namespace base
