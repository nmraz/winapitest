#include "cancellation.h"

namespace base {

bool cancellation_token::is_canceled() const {
  return canceled_->load(std::memory_order_acquire);
}

cancellation_token::cancellation_token(std::shared_ptr<const std::atomic<bool>> canceled)
  : canceled_(std::move(canceled)) {
}


cancellation_tracker::cancellation_tracker()
  : canceled_(std::make_shared<std::atomic<bool>>(false)) {
}

void cancellation_tracker::cancel() {
  canceled_->store(true, std::memory_order_release);
}

bool cancellation_tracker::is_canceled() const {
  return canceled_->load(std::memory_order_acquire);
}

cancellation_token cancellation_tracker::get_token() const {
  return cancellation_token(canceled_);
}

}  // namespace base