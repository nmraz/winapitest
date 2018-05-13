#pragma once

#include "base/event_loop/event_loop.h"
#include "base/win/message_window.h"
#include <atomic>

namespace base::win {

class win_event_loop : public event_loop {
public:
  win_event_loop();

  bool do_work() override;
  void sleep(const task::delay_type* delay) override;
  void wake_up() override;

private:
  LRESULT handle_message(UINT msg);

  void post_wake_msg();
  void clear_wake_flag();

  void kill_timer();
  void reschedule_timer();

  message_window message_window_;
  std::atomic<bool> posted_wake_up_;
  std::optional<task::run_time_type> current_next_run_time_;
};

}  // namespace base::win