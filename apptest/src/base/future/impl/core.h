#pragma once

#include "base/assert.h"
#include "base/function.h"
#include "base/future/exceptions.h"
#include "base/future/future_val.h"
#include "base/non_copyable.h"
#include <mutex>

namespace base::impl {

template<typename T>
class future_core : public non_copy_movable {
public:
  future_core() = default;
  future_core(future_val<T>&& val);

  template<typename Cont>
  void set_cont(Cont&& cont);
  void fulfill(future_val<T>&& val);

  bool is_fulfilled() const {
    return !val_.empty();
  }

private:
  void call_cont(std::unique_lock<std::mutex> hold) noexcept;

  future_val<T> val_;
  function<void(future_val<T>&&)> cont_;
  std::mutex lock_;  // protects val_, cont_
};


template<typename T>
future_core<T>::future_core(future_val<T>&& val)
  : val_(std::move(val)) {
}

template<typename T>
template<typename Cont>
void future_core<T>::set_cont(Cont&& cont) {
  std::unique_lock<std::mutex> hold(lock_);

  ASSERT(!cont_) << "Future continuation already set";
  cont_ = std::forward<Cont>(cont);

  call_cont(std::move(hold));
}

template<typename T>
void future_core<T>::fulfill(future_val<T>&& val) {
  std::unique_lock<std::mutex> hold(lock_);

  ASSERT(!is_fulfilled()) << "Promise already fulfilled";
  val_ = std::move(val);

  call_cont(std::move(hold));
}


// PRIVATE

template<typename T>
void future_core<T>::call_cont(std::unique_lock<std::mutex> hold) noexcept {
  if (cont_ && is_fulfilled()) {
    hold.unlock();

    // at this point, it is safe to access cont_ and val_ without locking,
    // since they are guaranteed not to be mutated elsewhere
    cont_(std::move(val_));
  }
}

}  // namespace base::impl