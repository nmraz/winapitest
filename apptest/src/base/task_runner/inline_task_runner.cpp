#include "inline_task_runner.h"

namespace base {
namespace {

class inline_task_runner : public task_runner {
  void do_post_task(task&& tsk) override {
    tsk.run();
  }
};

}  // namespace

std::shared_ptr<task_runner> get_inline_task_runner() {
  static auto runner = std::make_shared<inline_task_runner>();
  return runner;
}

}  // namespace base