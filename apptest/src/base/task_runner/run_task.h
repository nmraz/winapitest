#pragma once

#include "base/future/future.h"
#include "base/task_runner/task_runner.h"
#include <memory>

namespace base {

template<typename Cb>
auto run_task(std::shared_ptr<task_runner> runner, Cb&& callback)
  -> future<typename impl::is_future<std::decay_t<std::invoke_result_t<Cb&&>>>::inner_type> {
  return make_future()
    .then([callback = std::forward<Cb>(callback)](auto&&) mutable {
      return std::forward<Cb>(callback)();
    }, std::move(runner));
}

}  // namespace base