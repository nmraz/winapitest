#pragma once

#include "base/expected.h"
#include "base/future/impl/core.h"
#include "base/non_copyable.h"
#include "base/task_runner/task_runner.h"
#include <functional>
#include <memory>
#include <optional>

namespace base {

template<typename T>
class promise;

template<typename T>
class future;


namespace impl {

template<typename T>
struct unwrap_future {
  static constexpr bool is_future = false;
  static constexpr bool is_expected = false;
  using inner_type = T;
};

template<typename T>
struct unwrap_future<future<T>> {
  static constexpr bool is_future = true;
  static constexpr bool is_expected = false;
  using inner_type = T;
};

template<typename T>
struct unwrap_future<expected<T>> {
  static constexpr bool is_future = false;
  static constexpr bool is_expected = true;
  using inner_type = T;
};


// task runner used for posting future::then calls (may be different for different threads)
std::weak_ptr<task_runner> default_then_task_runner();

}  // namespace impl

template<typename T>
using unwrap_future_t = typename impl::unwrap_future<T>::inner_type;


template<typename T>
class promise : public non_copyable {
public:
  promise();
  promise(promise&& rhs) = default;
  ~promise();

  promise& operator=(promise rhs) noexcept;
  void swap(promise& other) noexcept;

  void set(expected<T> val);

  void set_exception(std::exception_ptr exc);
  template<typename Exc>
  void set_exception(Exc&& exc);

  template<
    typename U,
    typename T2 = T,
    typename = std::enable_if_t<!std::is_void_v<T2>>
  > void set_value(U&& val);

  template<
    typename T2 = T,
    typename = std::enable_if_t<std::is_void_v<T2>>
  > void set_value();
  
  template<typename F>
  void set_from(F&& f);

  future<T> get_future();

private:
  void check_valid() const {
    if (!core_) {
      throw no_future_state();
    }
  }

  std::shared_ptr<impl::future_core<T>> core_;
  bool future_retrieved_ = false;
};

template<typename T>
inline void swap(promise<T>& lhs, promise<T>& rhs) noexcept {
  lhs.swap(rhs);
}


template<typename T>
class future : public non_copyable {
public:
  template<typename T>
  friend future<T> make_future(expected<T> val);

  future() = default;
  void swap(future& other) noexcept;

  template<typename Cont>
  auto then(std::weak_ptr<task_runner> runner, Cont&& cont);

  template<typename Cont>
  auto then(Cont&& cont) {
    return then(impl::default_then_task_runner(), std::forward<Cont>(cont));
  }

  bool is_valid() const { return !!core_; }

private:
  template<typename T>
  friend class future;

  template<typename T>
  friend class promise;

  future(std::shared_ptr<impl::future_core<T>> core);

  void check_valid() const {
    if (!is_valid()) {
      throw no_future_state();
    }
  }

  template<typename Cont>
  void set_cont(Cont&& cont);

  std::shared_ptr<impl::future_core<T>> core_;
};

template<typename T>
inline void swap(future<T>& lhs, future<T>& rhs) noexcept {
  lhs.swap(rhs);
}


// IMPLEMENTATION

// promise<T>

template<typename T>
promise<T>::promise()
  : core_(std::make_shared<impl::future_core<T>>()) {
}

template<typename T>
promise<T>::~promise() {
  if (core_) {
    set_exception(abandoned_promise());
  }
}

template<typename T>
promise<T>& promise<T>::operator=(promise rhs) noexcept {
  rhs.swap(*this);
  return *this;
}

template<typename T>
void promise<T>::swap(promise& other) noexcept {
  using std::swap;
  swap(core_, other.core_);
  swap(future_retrieved_, other.future_retrieved_);
}

template<typename T>
void promise<T>::set(expected<T> val) {
  check_valid();
  core_->fulfill(std::move(val));
  core_ = nullptr;
}

template<typename T>
void promise<T>::set_exception(std::exception_ptr exc) {
  expected<T> val;
  val.set_exception(exc);
  set(std::move(val));
}

template<typename T>
template<typename Exc>
void promise<T>::set_exception(Exc&& exc) {
  set_exception(std::make_exception_ptr(std::forward<Exc>(exc)));
}

template<typename T>
template<typename U, typename T2, typename>
void promise<T>::set_value(U&& val) {  // enabled when T is non-void
  expected<T2> fut_val;
  fut_val.set_value(std::forward<U>(val));
  set(std::move(fut_val));
}

template<typename T>
template<typename T2, typename>
void promise<T>::set_value() {  // enabled when T is void
  expected<T2> val;
  val.set_value();
  set(std::move(val));
}

template<typename T>
template<typename F>
void promise<T>::set_from(F&& f) {
  using ret_type = std::decay_t<decltype(std::forward<F>(f)())>;
  using unwrapped_return = impl::unwrap_future<ret_type>;

  try {
    if constexpr (unwrapped_return::is_future) {  // future<T> return type
      std::forward<F>(f)().set_cont([self = std::move(*this)](auto&& cont_val) mutable {
        self.set(std::forward<decltype(cont_val)>(cont_val));
      });
    } else if constexpr (unwrapped_return::is_expected) {  // expected<T> return type
      set(std::forward<F>(f)());
    } else if constexpr (std::is_void_v<T>) {  // void return type
      std::forward<F>(f)();
      set_value();
    } else {  // other (non-future) return type
      set_value(std::forward<F>(f)());
    }
  } catch (...) {
    set_exception(std::current_exception());
  }
}

template<typename T>
future<T> promise<T>::get_future() {
  check_valid();
  if (future_retrieved_) {
    throw future_already_retrieved();
  }

  future_retrieved_ = true;
  return future<T>(core_);
}


// future<T>

namespace impl {

template<bool Wrap, typename Cont, typename T>
struct cont_call_ret {
  using type = std::invoke_result_t<Cont, expected<T>>;
};

template<typename Cont, typename T>
struct cont_call_ret<false, Cont, T> {
  using type = std::invoke_result_t<Cont, T>;
};

template<typename Cont, typename T>
struct cont_call_info {
  static constexpr bool call_wrapped = std::is_invocable_v<Cont, expected<T>>;
  static constexpr bool call_unwrapped = std::is_invocable_v<Cont, T>;

  static_assert(call_wrapped || call_unwrapped, "Illegal signature for future continuation");

  using ret_type = std::decay_t<typename cont_call_ret<call_wrapped, Cont, T>::type>;
};

template<typename Prom, typename Cont, typename T>
void call_cont(Prom& prom, Cont&& cont, expected<T>&& val) {
  using call_info = cont_call_info<Cont, T>;

  if constexpr (call_info::call_wrapped) {  // call_wrapped - pass `val` directly
    prom.set_from([&] {
      return std::invoke(std::forward<Cont>(cont), std::move(val));
    });
  } else {  // call_unwrapped - call `get()` on `val` first
    if (val.has_exception()) {
      prom.set_exception(val.get_exception());
    } else {
      prom.set_from([&] {
        if constexpr (std::is_void_v<T>) {
          return std::forward<Cont>(cont)();
        } else {
          return std::invoke(std::forward<Cont>(cont), std::move(val).get());
        }
      });
    }
  }
}

}  // namespace impl

template<typename T>
future<T>::future(std::shared_ptr<impl::future_core<T>> core)
  : core_(std::move(core)) {
}

template<typename T>
void future<T>::swap(future& other) noexcept {
  using std::swap;
  swap(core_, other.core_);
}

template<typename T>
template<typename Cont>
auto future<T>::then(std::weak_ptr<task_runner> runner, Cont&& cont) {
  check_valid();

  using result_type = typename impl::cont_call_info<Cont, T>::ret_type;

  promise<unwrap_future_t<result_type>> prom;
  auto fut = prom.get_future();

  set_cont([
    cont = std::forward<Cont>(cont),
    prom = std::move(prom),
    runner = std::move(runner)
  ](expected<T>&& val) mutable {
    if (auto strong_runner = runner.lock()) {
      strong_runner->post_task([
        cont = std::forward<Cont>(cont),
        prom = std::move(prom),
        val = std::move(val)
      ]() mutable {
        impl::call_cont(prom, std::forward<Cont>(cont), std::move(val));
      });
    }
  });

  return fut;
}

template<typename T>
template<typename Cont>
void future<T>::set_cont(Cont&& cont) {
  check_valid();
  core_->set_cont(std::forward<Cont>(cont));
  core_ = nullptr;
}


template<typename T>
future<T> make_future(expected<T> val) {
  auto core = std::make_shared<impl::future_core<T>>(std::move(val));
  return future<T>(std::move(core));
}

template<typename T>
future<std::decay_t<T>> make_future(T&& val) {
  expected<std::decay_t<T>> fut_val;
  fut_val.set_value(std::forward<T>(val));
  return make_future(std::move(fut_val));
}

inline future<void> make_future() {
  expected<void> val;
  val.set_value();
  return make_future(std::move(val));
}

}  // namespace base