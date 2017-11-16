#pragma once

#include "base/future/future_val.h"
#include "base/future/impl/core.h"
#include "base/non_copyable.h"
#include "base/task_runner/task_runner.h"
#include <memory>
#include <optional>

namespace base {

template<typename T>
class promise;

template<typename T>
class future;


namespace impl {

template<typename T>
struct is_future : std::false_type {
  using inner_type = T;
};

template<typename T>
struct is_future<future<T>> : std::true_type {
  using inner_type = T;
};


template<typename T, typename Cont>
void set_cont(future<T> fut, Cont&& cont) {
  fut.set_cont(std::forward<Cont>(cont));
}

// task runner used for posting future::then calls (may be different for different threads)
std::weak_ptr<task_runner> default_then_task_runner();

}  // namespace impl


template<typename T>
class promise : public non_copyable {
public:
  promise();
  promise(promise&& rhs) = default;
  ~promise();

  promise& operator=(promise rhs) noexcept;
  void swap(promise& other) noexcept;

  void set(future_val<T> val);

  void set_exception(std::exception_ptr exc);
  template<typename Exc>
  void set_exception(Exc&& exc);

  template<
    typename U,
    typename T2 = T,
    typename = std::enable_if_t<!std::is_void_v<T>>
  > void set_value(U&& val);

  template<
    typename T2 = T,
    typename = std::enable_if_t<std::is_void_v<T2>>
  > void set_value();
  
  template<typename F>
  void set_from(F&& f);

  future<T> get_future();
  bool is_fulfilled() const;

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
  friend future<T> make_future(future_val<T> val);

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
  friend class promise<T>;
  template<typename T>
  friend class future;
  template<typename T, typename Cont>
  friend void impl::set_cont(future<T> fut, Cont&& cont);

  future(std::shared_ptr<impl::future_core<T>> core);

  void check_valid() const {
    if (!is_valid()) {
      throw no_future_state();
    }
  }

  template<typename Cont>
  void set_cont(Cont&& cont);

  template<
    typename ReturnsFuture,
    typename Cont,
    typename Val
  > static void call_then_cont(Cont&& cont, Val&& val, promise<typename ReturnsFuture::inner_type> prom);

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
  if (core_ && !is_fulfilled()) {
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
void promise<T>::set(future_val<T> val) {
  check_valid();
  core_->fulfill(std::move(val));
}

template<typename T>
void promise<T>::set_exception(std::exception_ptr exc) {
  future_val<T> val;
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
  // to keep this dependant, use T2 instead of T
  future_val<T2> fut_val;
  fut_val.set_value(std::forward<U>(val));
  set(std::move(fut_val));
}

template<typename T>
template<typename T2, typename>
void promise<T>::set_value() {  // enabled when T is void
  // to keep this dependant, use T2 instead of void
  future_val<T2> val;
  val.set_value();
  set(std::move(val));
}

template<typename T>
template<typename F>
void promise<T>::set_from(F&& f) {
  try {
    if constexpr (std::is_void_v<T>) {
      std::forward<F>(f)();
      set_value();
    } else {
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

template<typename T>
bool promise<T>::is_fulfilled() const {
  check_valid();
  return core_->is_fulfilled();
}


// future<T>

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

  using result_type = std::decay_t<std::invoke_result_t<Cont&&, future_val<T>&&>>;
  using returns_future = impl::is_future<result_type>;

  promise<typename returns_future::inner_type> prom;
  auto fut = prom.get_future();

  set_cont([
    cont = std::forward<Cont>(cont),
    prom = std::move(prom),
    runner = std::move(runner)
  ](future_val<T>&& val) mutable {
    if (auto strong_runner = runner.lock()) {
      strong_runner->post_task([
        cont = std::forward<Cont>(cont),
        prom = std::move(prom),
        val = std::move(val)
      ]() mutable {
        call_then_cont<returns_future>(std::forward<Cont>(cont), std::move(val), std::move(prom));
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
template<typename ReturnsFuture, typename Cont, typename Val>
void future<T>::call_then_cont(Cont&& cont, Val&& val, promise<typename ReturnsFuture::inner_type> prom) {
  if constexpr (ReturnsFuture::value) {  // future return type
    try {
      std::forward<Cont>(cont)(std::move(val))
        .set_cont([prom = std::move(prom)](auto&& cont_val) mutable {
          prom.set(std::forward<decltype(cont_val)>(cont_val));
        });
    } catch (...) {
      prom.set_exception(std::current_exception());
    }
  } else {  // non-future return type
    prom.set_from([&]() mutable {
      return std::forward<Cont>(cont)(std::move(val));
    });
  }
}


template<typename T>
future<T> make_future(future_val<T> val) {
  auto core = std::make_shared<impl::future_core<T>>(std::move(val));
  return future<T>(core);
}

template<typename T>
future<std::decay_t<T>> make_future(T&& val) {
  future_val<std::decay_t<T>> fut_val;
  val.set_value(std::forward<T>(val));
  return make_future(std::move(val));
}

inline future<void> make_future() {
  future_val<void> val;
  val.set_value();
  return make_future(std::move(val));
}

}  // namespace base