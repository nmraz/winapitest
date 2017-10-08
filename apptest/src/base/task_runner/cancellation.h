#pragma once

#include "base/non_copyable.h"
#include <atomic>
#include <memory>

namespace base {

class cancellation_tracker;

class cancellation_token {
public:
  bool is_canceled() const;

private:
  friend cancellation_tracker;
  explicit cancellation_token(std::shared_ptr<const std::atomic<bool>> canceled);

  std::shared_ptr<const std::atomic<bool>> canceled_;
};


class cancellation_tracker : non_copyable {
public:
  cancellation_tracker();

  void cancel();
  bool is_canceled() const;

  cancellation_token get_token() const;

private:
  std::shared_ptr<std::atomic<bool>> canceled_;
};

}  // namespace base
