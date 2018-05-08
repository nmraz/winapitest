#pragma once

#include "base/assert.h"
#include "base/expected.h"
#include "base/function.h"
#include "base/future/exceptions.h"
#include "base/non_copyable.h"
#include <exception>
#include <mutex>

namespace base::impl {

template<typename T>
class future_core : public non_copy_movable {
public:
  future_core() = default;
  future_core(expected<T>&& val);

  ~future_core();

  template<typename Cont>
  void set_cont(Cont&& cont);
  void fulfill(expected<T>&& val);

private:
  void call_cont(std::unique_lock<std::mutex> hold) noexcept;

  expected<T> val_;
  function<void(expected<T>&&)> cont_;
  std::mutex lock_;  // protects val_, cont_

  bool cont_set_ = false;
  bool fulfilled_ = false;
};


template<typename T>
future_core<T>::future_core(expected<T>&& val)
  : val_(std::move(val))
  , fulfilled_(true) {
}

template<typename T>
future_core<T>::~future_core() {
  if (val_.has_exception()) {
    std::terminate();
  }
}

template<typename T>
template<typename Cont>
void future_core<T>::set_cont(Cont&& cont) {
  std::unique_lock hold(lock_);

  ASSERT(!cont_set_) << "Future continuation already set";
  cont_ = std::forward<Cont>(cont);
  cont_set_ = true;

  call_cont(std::move(hold));
}

template<typename T>
void future_core<T>::fulfill(expected<T>&& val) {
  std::unique_lock hold(lock_);

  ASSERT(!fulfilled_) << "Promise already fulfilled";
  val_ = std::move(val);
  fulfilled_ = true;

  call_cont(std::move(hold));
}


// PRIVATE

template<typename T>
void future_core<T>::call_cont(std::unique_lock<std::mutex> hold) noexcept {
  if (cont_set_ && fulfilled_) {
    hold.unlock();

    // at this point, it is safe to access cont_ and val_ without locking,
    // since they are guaranteed not to be mutated elsewhere
    cont_(std::move(val_));
    val_.reset();
  }
}

}  // namespace base::impl