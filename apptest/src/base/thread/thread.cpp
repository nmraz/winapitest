#include "thread.h"

#include "base/assert.h"
#include "base/thread/thread_name.h"
#include <utility>

namespace base {
namespace {

thread_local bool quit_properly = false;

}  // namespace

thread::thread(loop_factory factory)
  : thread_(&thread::run, this, std::move(factory)) {
}

thread::thread(loop_factory factory, std::string name)
  : thread_(&thread::named_run, this, std::move(factory), std::move(name)) {
}

thread::~thread() {
  stop();
}


void thread::stop(bool wait) {
  if (thread_.joinable()) {
    task_runner()->post_task([] {
      quit_properly = true;
      loop_task_runner::current()->quit_now();
    });
    wait ? thread_.join() : thread_.detach();
  }
}


loop_task_runner::ptr thread::task_runner() const {
  std::unique_lock hold(runner_lock_);
  runner_cv_.wait(hold, [this] { return !!runner_; });  // wait until the runner exists
  return runner_;
}


std::string thread::name() const {
  return get_thread_name(get_id());
}


// PRIVATE

void thread::run(loop_factory factory) {
  loop_task_runner::init_for_this_thread();
  set_task_runner(loop_task_runner::current());
  std::unique_ptr<event_loop> loop = factory();
  loop->run();
  ASSERT(quit_properly) << "Thread should not quit of its own accord";
}

void thread::named_run(loop_factory factory, std::string name) {
  set_current_thread_name(std::move(name));
  run(std::move(factory));
}


void thread::set_task_runner(loop_task_runner::ptr runner) {
  std::lock_guard hold(runner_lock_);
  runner_ = std::move(runner);
  runner_cv_.notify_all();
}

}  // namepsace base