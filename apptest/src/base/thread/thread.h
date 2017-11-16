#pragma once

#include "base/function.h"
#include "base/event_loop/event_loop.h"
#include "base/event_loop/loop_task_runner.h"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace base {

class thread {
public:
  using loop_factory = function<std::unique_ptr<event_loop>()>;

  thread(loop_factory factory);
  thread(loop_factory factory, std::string name);
  ~thread();

  void stop(bool wait = true);

  loop_task_runner::ptr task_runner() const;

  std::thread::id get_id() const { return thread_.get_id(); }
  std::string name() const;

private:
  void run(loop_factory factory);
  void named_run(loop_factory factory, std::string name);

  void set_task_runner(loop_task_runner::ptr runner);

  // wait mechanism for task_runner
  mutable std::mutex runner_lock_;
  mutable std::condition_variable runner_cv_;

  loop_task_runner::ptr runner_;
  std::thread thread_;  // the thread must be constructed (and started) last!
};

}  // namespace base