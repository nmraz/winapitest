#include "event_loop.h"

#include "base/assert.h"
#include "base/auto_restore.h"
#include "base/event_loop/loop_task_runner.h"
#include <algorithm>

namespace base {
namespace {

thread_local event_loop* current_loop = nullptr;
thread_local int current_nesting_level = 0;

}  // namespace



struct event_loop::loop_pusher {
  loop_pusher(event_loop* loop);
  ~loop_pusher();


  event_loop* prev_loop_;
};

event_loop::loop_pusher::loop_pusher(event_loop* loop)
  : prev_loop_(current_loop) {
  ASSERT(loop->runner_ == loop_task_runner::current().get()) << "Attempting to run event loop on wrong thread";
  loop->runner_->set_loop(loop);
  current_loop = loop;
}

event_loop::loop_pusher::~loop_pusher() {
  ASSERT(current_loop) << "Attempting to pop non-existant loop";
  current_loop->runner_->set_loop(prev_loop_);
  current_loop = prev_loop_;
}


event_loop::event_loop()
  : runner_(loop_task_runner::current().get()) {
}


void event_loop::run() {
  loop_pusher push(this);
  auto_restore restore_nesting(current_nesting_level);

  ++current_nesting_level;
  should_quit_ = false;

  while (true) {
    bool ran_task = do_work();
    if (should_quit_) {
      break;
    }

    ran_task |= run_pending_task();
    if (should_quit_) {
      break;
    }

    ran_task |= run_delayed_task();
    if (should_quit_) {
      break;
    }

    if (!ran_task) {
      auto next_run_time = get_next_run_time();
      if (next_run_time) {
        auto delay = *next_run_time - task::clock_type::now();
        if (delay > task::delay_type::zero()) {
          sleep(&delay);
        }
      } else {
        sleep(nullptr);
      }
    }
  }
}

void event_loop::quit() {
  should_quit_ = true;
}


bool event_loop::do_work() {
  return false;
}


bool event_loop::is_current() const {
  return this == current_loop;
}


// static
event_loop* event_loop::current() {
  return current_loop;
}

// static
int event_loop::nesting_level() {
  return current_nesting_level;
}


// PROTECTED

bool event_loop::run_pending_task() {
  return get_runner()->run_pending_task();
}

bool event_loop::run_delayed_task() {
  return get_runner()->run_delayed_task();
}

std::optional<task::run_time_type> event_loop::get_next_run_time() const {
  return get_runner()->get_next_run_time();
}


// PRIVATE

loop_task_runner* event_loop::get_runner() const {
  ASSERT(is_current()) << "Only the active event_loop can run tasks";
  return runner_;
}

}  // namespace base