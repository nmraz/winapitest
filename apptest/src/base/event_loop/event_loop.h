#pragma once

#include "base/task_runner/task.h"
#include "base/non_copyable.h"
#include <optional>

namespace base {

class loop_task_runner;

class event_loop : public non_copy_movable {
public:
  event_loop();
  virtual ~event_loop() = default;

  void run();
  void quit();

  virtual bool do_work();
  virtual void sleep(const task::delay_type* delay) = 0;
  virtual void wake_up() = 0;

  bool is_current() const;

  static event_loop& current();
  static bool is_nested();

protected:
  bool run_pending_task();
  bool run_delayed_task();
  std::optional<task::run_time_type> get_next_run_time() const;

private:
  struct loop_pusher;

  // Check that the loop is active and retrieve the current loop_task_runner
  loop_task_runner* get_runner() const;

  bool should_quit_;
  loop_task_runner* runner_;  // the runner should outlive us anyway
};

}  // namespace base
