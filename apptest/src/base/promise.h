#pragma once

#include "base/assert.h"
#include "base/non_copyable.h"
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <variant>

namespace base {

class bad_promise_val : public std::logic_error {
public:
  bad_promise_val();
};

class abandoned_promise : public std::logic_error {
public:
  abandoned_promise();
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
  template<typename Cont>
  void set_cont(Cont&& cont);

  template<typename State>
  void set_state(State&& state);

  bool is_ready() const;
  void call_cont() noexcept;

  const typename promise_state<T>::rep_type& raw_state() const { return state_.rep; }

private:
  promise_state<T> state_;
  std::function<void(promise_state<T>&&)> cont_;
};

template<typename T>
template<typename Cont>
void promise_data<T>::set_cont(Cont&& cont) {
  ASSERT(!cont_) << "Promise continuation already set";
  cont_ = std::forward<Cont>(cont);

  if (is_ready()) {
    call_cont();
  }
}

template<typename T>
template<typename State>
void promise_data<T>::set_state(State&& state) {
  if (!std::holds_alternative<promise_state_pending>(state_.rep)) {
    return;
  }

  state_.rep = std::forward<State>(state);
  call_cont();
}

template<typename T>
void promise_data<T>::call_cont() noexcept {
  if (cont_) {
    cont_(std::move(state_));
    cont_ = nullptr;
  }
}

template<typename T>
bool promise_data<T>::is_ready() const {
  return std::holds_alternative<promise_state_resolved<T>>(state_.rep)
    || std::holds_alternative<promise_state_rejected>(state_.rep);
}


template<typename T>
class promise_source_base : non_copyable {
public:
  promise_source_base();
  ~promise_source_base();

  void swap(promise_source_base& other) noexcept;

  promise<T> get_promise() const;
  void set_exception(std::exception_ptr exc);

protected:
  std::shared_ptr<promise_data<T>> data_;

private:
  template<typename T>
  friend void copy_promise_data(promise_source_base<T>& source, promise<T> prom);
};


template<typename T>
void copy_promise_data(promise_source_base<T>& source, promise<T> prom) {
  source.data_ = prom.data_;
}

}  // namespace impl


template<typename T>
class promise_val : public non_copyable {
public:
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
class promise {
public:
  promise() = default;
  void swap(promise& other) noexcept;

  bool is_valid() const;

  template<typename Cont>
  auto then(Cont&& cont);

private:
  template<typename T>
  friend void impl::copy_promise_data(promise_source_base<T>& source, promise<T> prom);

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
  template<typename U>
  void set_value(U&& val);
};

template<>
class promise_source<void> : public impl::promise_source_base<void> {
public:
  void set_value();
};

template<typename T>
inline void swap(promise_source<T>& lhs, promise_source<T>& rhs) {
  lhs.swap(rhs);
}


template<typename T>
promise<std::decay_t<T>> make_resolved_promise(T&& val) {
  promise_source<std::decay_t<T>> source;
  source.set_value(std::forward<T>(val));
  return source.get_promise();
}

template<typename T>
promise<T> make_resolved_promise(promise<T> val) {
  return val;
}

inline promise<void> make_resolved_promise() {
  promise_source<void> source;
  source.set_value();
  return source.get_promise();
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
  if (std::holds_alternative<promise_state_pending>(data_->raw_state())) {  // the promise has been abandoned
    set_exception(std::make_exception_ptr(abandoned_promise()));
  }
}

template<typename T>
void promise_source_base<T>::swap(promise_source_base& other) noexcept {
  using std::swap;
  swap(data_, other.data_);
}

template<typename T>
promise<T> promise_source_base<T>::get_promise() const {
  return promise<T>(data_);
}

template<typename T>
void promise_source_base<T>::set_exception(std::exception_ptr exc) {
  ASSERT(data_) << "Empty promise";

  data_->set_state(promise_state_rejected{ exc });
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

template<typename T>
using promise_source_for = promise_source<remove_promise<T>>;

template<typename Ret>
struct promise_cont_caller {
  template<typename Cont, typename Arg>
  static void call(promise_source<Ret>& source, Cont&& cont, Arg&& arg) {
    source.set_value(std::forward<Cont>(cont)(std::forward<Arg>(arg)));
  }
};

template<typename Ret>
struct promise_cont_caller<promise<Ret>> {
  template<typename Cont, typename Arg>
  static void call(promise_source<Ret>& source, Cont&& cont, Arg&& arg) {
    copy_promise_data(source, std::forward<Cont>(cont)(std::forward<Arg>(arg)));
  }
};

template<>
struct promise_cont_caller<void> {
  template<typename Cont, typename Arg>
  static void call(promise_source<void>& source, Cont&& cont, Arg&& arg) {
    std::forward<Cont>(cont)(std::forward<Arg>(arg));
    source.set_value();
  }
};

}  // namespace impl

template<typename T>
template<typename Cont>
auto promise<T>::then(Cont&& cont) {
  using cont_result_type = std::decay_t<std::invoke_result_t<Cont&&, promise_val<T>&&>>;

  auto source = std::make_shared<impl::promise_source_for<cont_result_type>>();

  data_->set_cont([source, cont = std::forward<Cont>(cont)](impl::promise_state<T>&& state) mutable {
    promise_val<T> val(std::move(state));

    try {
      impl::promise_cont_caller<cont_result_type>::call(*source, std::forward<Cont>(cont), std::move(val));
    } catch (...) {
      source->set_exception(std::current_exception());
    }
  });

  return source->get_promise();
}


// promise_source<T>

template<typename T>
template<typename U>
void promise_source<T>::set_value(U&& val) {
  this->data_->set_state(impl::promise_state_resolved<T>{ std::forward<U>(val) });
}


// promise_source<void>

inline void promise_source<void>::set_value() {
  data_->set_state(impl::promise_state_resolved<void>{});
}

}  // namespace base