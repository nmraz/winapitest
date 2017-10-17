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
std::shared_ptr<task_runner> default_then_task_runner();

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
      throw no_state();
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
  auto then(Cont&& cont, std::shared_ptr<task_runner> runner);

  template<typename Cont>
  auto then(Cont&& cont) {
    return then(std::forward<Cont>(cont), impl::default_then_task_runner());
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
      throw no_state();
    }
  }

  template<typename Cont>
  void set_cont(Cont&& cont);

  template<
    typename Cont,
    bool ReturnsFuture,
    typename Context
  > static void call_then_cont(Context ctx);

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
auto future<T>::then(Cont&& cont, std::shared_ptr<task_runner> runner) {
  check_valid();

  using result_type = std::decay_t<std::invoke_result_t<Cont&&, future_val<T>&&>>;
  using returns_future = impl::is_future<result_type>;
  using promise_type = promise<typename returns_future::inner_type>;

  struct then_context {
    future_val<T> val;
    promise_type prom;
    std::optional<std::decay_t<Cont>> cont;  // allows control over destruction of cont
  };

  auto ctx = std::make_shared<then_context>(then_context{ {}, {}, std::forward<Cont>(cont) });
  auto fut = ctx->prom.get_future();

  set_cont([ctx = std::move(ctx), runner = std::move(runner)](future_val<T>&& val) mutable {
    ctx->val = std::move(val);
    runner->post_task([ctx = std::move(ctx)]() mutable {
      call_then_cont<Cont, returns_future::value>(std::move(ctx));
    });
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
template<typename Cont, bool ReturnsFuture, typename Context>
void future<T>::call_then_cont(Context ctx) {
  if constexpr (ReturnsFuture) {  // future return type
    try {
      auto ret = std::forward<Cont>(*ctx->cont)(std::move(ctx->val));

      // clean up - the context must stay alive (for the promise),
      // but its other members need to be destroyed
      ctx->val.reset();
      ctx->cont.reset();

      ret.set_cont([ctx = std::move(ctx)](auto&& val) {
        ctx->prom.set(std::forward<decltype(val)>(val));
      });
    } catch (...) {
      ctx->prom.set_exception(std::current_exception());
    }
  } else {  // non-future return type
    ctx->prom.set_from([&ctx] {
      return std::forward<Cont>(*ctx->cont)(std::move(ctx->val));
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