#pragma once

#include "base/future/future.h"
#include "base/task_runner/task_runner.h"
#include <memory>

namespace base {

template<typename Cb>
auto run_task(task_runner& runner, Cb&& callback) {
  using ret_type = std::decay_t<decltype(std::forward<Cb>(callback)())>;

  promise<unwrap_future_t<ret_type>> prom;
  auto fut = prom.get_future();
  
  runner.post_task([callback = std::forward<Cb>(callback), prom = std::move(prom)]() mutable {
    prom.set_from([&callback] {
      return std::forward<Cb>(callback)();
    });
  });

  return fut;
}

}  // namespace base