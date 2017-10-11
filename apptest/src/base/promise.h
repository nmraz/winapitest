#pragma once

#include "base/assert.h"
#include "base/non_copyable.h"
#include "base/task_runner/inline_task_runner.h"
#include "base/task_runner/task_runner.h"
#include <exception>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <type_traits>
#include <variant>
#include <vector>

namespace base {

class bad_promise_val : public std::logic_error {
public:
  bad_promise_val();
};

class abandoned_promise : public std::logic_error {
public:
  abandoned_promise();
};

class promise_already_retrieved : public std::logic_error {
public:
  promise_already_retrieved();
};


template<typename T>
class promise_source;

template<typename T>
class promise;

template<typename T>
class promise_val;


namespace impl {

struct promise_state_pending {};  // indicates that the promise is waiting for a value
struct promise_state_moved {};  // indicates that the promise has been moved from

template<typename T>
struct promise_state_resolved {  // indicates that the promise has been resolved
  T val;
};

template<>
struct promise_state_resolved<void> {};

struct promise_state_rejected {  // indicates that the promise has been rejected
  std::exception_ptr exc;
};

template<typename T>
struct promise_state : non_copyable {
  using rep_type = std::variant<
    promise_state_pending,
    promise_state_resolved<T>,
    promise_state_rejected,
    promise_state_moved
  >;

  constexpr promise_state() = default;
  promise_state(promise_state&& rhs) noexcept;

  ~promise_state();

  promise_state& operator=(promise_state rhs) noexcept;

  rep_type move_rep();

  rep_type rep;
};

template<typename T>
promise_state<T>::promise_state(promise_state&& rhs) noexcept
  : rep(std::move(rhs.rep)) {
  rhs.rep = promise_state_moved{};
}

template<typename T>
promise_state<T>::~promise_state() {
  if (std::holds_alternative<promise_state_rejected>(rep)) {  // unhandled exception (rejection)
    std::terminate();
  }
}

template<typename T>
promise_state<T>& promise_state<T>::operator=(promise_state rhs) noexcept {
  using std::swap;
  swap(rep, rhs.rep);
  return *this;
}

template<typename T>
typename promise_state<T>::rep_type promise_state<T>::move_rep() {
  auto tmp = std::move(rep);
  rep = promise_state_moved{};
  return tmp;
}


template<typename T>
struct promise_data {
public:
  promise_data() = default;
  template<typename State>
  promise_data(State&& state);

  template<typename Cont>
  void set_cont(Cont&& cont);

  template<typename State>
  void fulfill(State&& state);

private:
  bool is_ready() const;
  bool is_pending() const;

  void call_cont(std::unique_lock<std::mutex>& hold) noexcept;

  promise_state<T> state_;
  std::function<void(promise_state<T>&&)> cont_;
  std::mutex lock_;  // protects state_, cont_
};

template<typename T>
template<typename State>
promise_data<T>::promise_data(State&& state) {
  state_.rep = std::forward<State>(state);
}

template<typename T>
template<typename Cont>
void promise_data<T>::set_cont(Cont&& cont) {
  std::unique_lock<std::mutex> hold(lock_);

  ASSERT(!cont_) << "Promise continuation already set";
  cont_ = std::forward<Cont>(cont);

  if (is_ready()) {
    call_cont(hold);
  }
}

template<typename T>
template<typename State>
void promise_data<T>::fulfill(State&& state) {
  std::unique_lock<std::mutex> hold(lock_);

  if (!is_pending()) {
    return;
  }

  state_.rep = std::forward<State>(state);
  call_cont(hold);
}

template<typename T>
bool promise_data<T>::is_ready() const {
  return std::holds_alternative<promise_state_resolved<T>>(state_.rep)
    || std::holds_alternative<promise_state_rejected>(state_.rep);
}

template<typename T>
bool promise_data<T>::is_pending() const {
  return std::holds_alternative<promise_state_pending>(state_.rep);
}

template<typename T>
void promise_data<T>::call_cont(std::unique_lock<std::mutex>& hold) noexcept {
  if (auto cont = std::exchange(cont_, nullptr)) {
    auto state = std::move(state_);
    hold.unlock();

    // note: continuation is called *outside* the lock
    cont(std::move(state));
  }
}


template<typename T>
class promise_source_base : non_copyable {
public:
  using result_type = T;
  using promise_type = promise<T>;

  promise_source_base();
  promise_source_base(promise_source_base&& rhs) = default;
  ~promise_source_base();

  void swap(promise_source_base& other) noexcept;

  promise<T> get_promise();

  void set_value(promise<T> prom);

  template<typename Exc>
  void set_exception(Exc&& exc);
  void set_exception(std::exception_ptr exc);

protected:
  std::shared_ptr<promise_data<T>> data_;

private:
  void abandon();

  bool promise_retrieved_ = false;
};

}  // namespace impl


template<typename T>
class promise_val : public non_copyable {
public:
  using value_type = T;

  constexpr promise_val() = default;
  promise_val(impl::promise_state<T>&& state);

  void swap(promise_val& other) noexcept;

  T get();

private:
  impl::promise_state<T> state_;
};

template<typename T>
inline void swap(promise_val<T>& lhs, promise_val<T>& rhs) {
  lhs.swap(rhs);
}


template<typename T>
class promise : public non_copyable {
public:
  using result_type = T;
  using source_type = promise_source<T>;

  promise() = default;
  void swap(promise& other) noexcept;

  bool is_valid() const;

  template<typename Cont>
  auto then(Cont&& cont);

  template<typename Cont>
  auto then(Cont&& cont, std::shared_ptr<task_runner> runner);

private:
  template<typename T>
  friend auto make_resolved_promise(T&& val);
  friend promise<void> make_resolved_promise();
  friend class impl::promise_source_base<T>;

  promise(std::shared_ptr<impl::promise_data<T>> data);

  std::shared_ptr<impl::promise_data<T>> data_;
};

template<typename T>
inline void swap(promise<T>& lhs, promise<T>& rhs) {
  lhs.swap(rhs);
}


template<typename T>
class promise_source : public impl::promise_source_base<T> {
public:
  promise_source() = default;
  promise_source(promise_source&& rhs) = default;

  promise_source& operator=(promise_source&& rhs) noexcept;

  using impl::promise_source_base<T>::set_value;
  void set_value(T val);
};

template<>
class promise_source<void> : public impl::promise_source_base<void> {
public:
  promise_source() = default;
  promise_source(promise_source&& rhs) = default;

  promise_source& operator=(promise_source&& rhs) noexcept;

  using impl::promise_source_base<void>::set_value;
  void set_value();
};

template<typename T>
inline void swap(promise_source<T>& lhs, promise_source<T>& rhs) {
  lhs.swap(rhs);
}



// UTILITIES

namespace impl {

template<typename T>
struct remove_promise_impl {
  using type = T;
};

template<typename T>
struct remove_promise_impl<promise<T>> {
  using type = T;
};

template<typename T>
using remove_promise = typename remove_promise_impl<T>::type;

}  // namespace impl

template<typename T>
using unwrapped_promise_source = promise_source<impl::remove_promise<T>>;

template<typename T>
using unwrapped_promise = promise<impl::remove_promise<T>>;


template<typename T>
auto make_resolved_promise(T&& val) {
  using decayed_t = std::decay_t<T>;

  auto data = std::make_shared<impl::promise_data<decayed_t>>(
    impl::promise_state_resolved<T>{ std::forward<T>(val) }
  );
  return promise<decayed_t>(std::move(data));
}

template<typename T>
promise<T> make_resolved_promise(promise<T> val) {
  return val;
}

inline promise<void> make_resolved_promise() {
  auto data = std::make_shared<impl::promise_data<void>>(
    impl::promise_state_resolved<void>{}
  );
  return promise<void>(std::move(data));
}


namespace impl {

template<typename T>
constexpr bool is_promise = false;
template<typename T>
constexpr bool is_promise<promise<T>> = true;

template<typename T>
struct when_all_impl {
  template<typename InIt>
  static promise<std::vector<T>> call(InIt begin, InIt end) {
    if (begin == end) {
      return make_resolved_promise(std::vector<T>());
    }

    auto ret = std::make_shared<std::vector<T>>();

    // note: std::accumulate requires that elements aren't mutated
    auto acc = make_resolved_promise();
    for (; begin != end; ++begin) {
      acc = acc.then([prom = std::move(*begin)](promise_val<void> val) mutable {
        val.get();  // propagate any exceptions
        return std::move(prom);
      }, get_inline_task_runner()).then([ret](promise_val<T> val) mutable {
        // no need to synchronize here since we wait sequentially
        ret->push_back(val.get());
      }, get_inline_task_runner());
    }

    return acc.then([ret](promise_val<void> val) {
      val.get();  // propagate exceptions
      return std::move(*ret);
    }, get_inline_task_runner());
  }
};

template<>
struct when_all_impl<void> {
  template<typename InIt>
  static promise<void> call(InIt begin, InIt end) {
    if (begin == end) {
      return make_resolved_promise();
    }

    auto acc = make_resolved_promise();
    for (; begin != end; ++begin) {
      acc = acc.then([prom = std::move(*begin)](promise_val<void> val) mutable {
        val.get();  // propagate any exceptions
        return std::move(prom);
      }, get_inline_task_runner());
    }

    return acc;
  }
};

template<typename T>
struct when_any_impl {
  template<typename InIt>
  static promise<T> call(InIt begin, InIt end) {
    promise_source<T> source;
    for (; begin != end; ++begin) {
      source.set_value(std::move(*begin));
    }
    return source.get_promise();
  }
};

}  // namespace impl

template<typename InIt>
auto when_all(InIt begin, InIt end) {
  using it_val = typename std::iterator_traits<InIt>::value_type;
  static_assert(impl::is_promise<it_val>, "when_all requires iterators to promises");

  return impl::when_all_impl<typename it_val::result_type>::call(begin, end);
}

template<typename InIt>
auto when_any(InIt begin, InIt end) {
  using it_val = typename std::iterator_traits<InIt>::value_type;
  static_assert(impl::is_promise<it_val>, "when_any requires iterators to promises");

  return impl::when_any_impl<typename it_val::result_type>::call(begin, end);
}



// IMPLEMENTATION

namespace impl {

// promise_source_base<T>

template<typename T>
promise_source_base<T>::promise_source_base()
  : data_(std::make_shared<promise_data<T>>()) {
}

template<typename T>
promise_source_base<T>::~promise_source_base() {
  abandon();
}

template<typename T>
void promise_source_base<T>::swap(promise_source_base& other) noexcept {
  using std::swap;
  swap(data_, other.data_);
}

template<typename T>
promise<T> promise_source_base<T>::get_promise() {
  ASSERT(data_) << "No state";

  if (promise_retrieved_) {
    throw promise_already_retrieved();
  }
  promise_retrieved_ = true;
  return promise<T>(data_);
}

template<typename T>
void promise_source_base<T>::set_value(promise<T> prom) {
  ASSERT(prom.is_valid()) << "No state";
  ASSERT(data_) << "No state";
  prom.data_->set_cont([data = data_](promise_state<T>&& state) {
    data->fulfill(state.move_rep());
  });
}

template<typename T>
template<typename Exc>
void promise_source_base<T>::set_exception(Exc&& exc) {
  set_exception(std::make_exception_ptr(std::forward<Exc>(exc)));
}

template<typename T>
void promise_source_base<T>::set_exception(std::exception_ptr exc) {
  ASSERT(data_) << "No state";
  data_->fulfill(promise_state_rejected{ exc });
}

template<typename T>
void promise_source_base<T>::abandon() {
  if (!data_) {
    return;
  }

  set_exception(abandoned_promise());  // this will have no effect if the promise has already been fulfilled
  data_ = nullptr;
}

}  // namespace impl


// promise_val<T>

template<typename T>
void promise_val<T>::swap(promise_val& other) noexcept {
  using std::swap;
  swap(state_.rep, rhs.state_.rep);
}

namespace impl {

template<typename T>
struct promise_state_visitor {
  T operator()(promise_state_rejected&& rej) {
    std::rethrow_exception(rej.exc);
  }

  T operator()([[maybe_unused]] promise_state_resolved<T>&& res) {
    if constexpr (!std::is_void_v<T>) {
      return std::move(res.val);
    }
  }

  template<typename U>
  T operator()(U&&) {
    throw bad_promise_val();
  }
};

}  // namespace impl

template<typename T>
T promise_val<T>::get() {
  return std::visit(impl::promise_state_visitor<T>{}, state_.move_rep());
}

template<typename T>
promise_val<T>::promise_val(impl::promise_state<T>&& state)
  : state_(std::move(state)) {
}


// promise<T>

template<typename T>
void promise<T>::swap(promise& other) noexcept {
  using std::swap;
  swap(data_, other.data_);
}

template<typename T>
bool promise<T>::is_valid() const {
  return data_ != nullptr;
}

template<typename T>
promise<T>::promise(std::shared_ptr<impl::promise_data<T>> data)
  : data_(std::move(data)) {
}

namespace impl {

std::shared_ptr<task_runner> default_promise_task_runner();

template<typename Ret, typename Source, typename Cont, typename Arg>
void call_promise_cont(Source& source, Cont&& cont, Arg&& arg) {
  if constexpr (std::is_void_v<Ret>) {
    std::forward<Cont>(cont)(std::forward<Arg>(arg));
    source.set_value();
  } else {
    source.set_value(std::forward<Cont>(cont)(std::forward<Arg>(arg)));
  }
}

}  // namespace impl

template<typename T>
template<typename Cont>
auto promise<T>::then(Cont&& cont) {
  return then(std::forward<Cont>(cont), impl::default_promise_task_runner());
}

template<typename T>
template<typename Cont>
auto promise<T>::then(Cont&& cont, std::shared_ptr<task_runner> runner) {
  using cont_result_type = std::decay_t<std::invoke_result_t<Cont&&, promise_val<T>&&>>;

  struct cont_context {
    unwrapped_promise_source<cont_result_type> source;
    impl::promise_state<T> state;
    std::decay_t<Cont> cont;
  };

  ASSERT(is_valid()) << "No state";

  auto ctx = std::make_shared<cont_context>(cont_context{ {}, {}, std::forward<Cont>(cont) });

  data_->set_cont([
    runner = std::move(runner),
    ctx
  ](impl::promise_state<T>&& state) mutable {
    ctx->state = std::move(state);

    runner->post_task([ctx = std::move(ctx)]() mutable {
      try {
        impl::call_promise_cont<cont_result_type>(ctx->source, std::forward<Cont>(ctx->cont), std::move(ctx->state));
      } catch (...) {
        ctx->source.set_exception(std::current_exception());
      }
    });
  });
  data_ = nullptr;

  return ctx->source.get_promise();
}


// promise_source<T>

template<typename T>
promise_source<T>& promise_source<T>::operator=(promise_source&& rhs) noexcept {
  promise_source(std::move(rhs)).swap(*this);
}

template<typename T>
void promise_source<T>::set_value(T val) {
  ASSERT(this->data_) << "No state";
  this->data_->fulfill(impl::promise_state_resolved<T>{ std::move(val) });
}


// promise_source<void>

inline promise_source<void>& promise_source<void>::operator=(promise_source&& rhs) noexcept {
  promise_source(std::move(rhs)).swap(*this);
}

inline void promise_source<void>::set_value() {
  ASSERT(data_) << "No state";
  data_->fulfill(impl::promise_state_resolved<void>{});
}

}  // namespace base