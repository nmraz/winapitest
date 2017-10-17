#pragma once

#include "base/future/future.h"
#include "base/task_runner/task_runner.h"
#include <memory>

namespace base {

template<typename Cb>
auto run_task(std::shared_ptr<task_runner> runner, Cb&& callback) {
  return make_future()
    .then([callback = std::forward<Cb>(callback)](auto&&) mutable {
      return std::forward<Cb>(callback)();
    }, std::move(runner));
}

}  // namespace base