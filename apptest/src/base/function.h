#pragma once

#include "base/assert.h"
#include "base/non_copyable.h"
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

// base::function is similar in design and purpose to std::function,
// but doesn't require copyability (and hence is move-only itself). It
// is also noexcept movable/swappable, unlike std::function.

namespace base {

template<typename Sig>
class function;


namespace impl {

constexpr int func_space_ptr_size = 7;  // base::function is 8 pointers total together with the impl_ pointer
using func_space = std::aligned_union_t<func_space_ptr_size * sizeof(void*), void*>;

template<typename Impl>
constexpr bool needs_heap =
  sizeof(Impl) > sizeof(func_space)  // too big
  || alignof(func_space) % alignof(Impl) != 0  // over-aligned
  || !std::is_nothrow_move_constructible_v<Impl>;  // can't guarentee noexcept move/swap


template<typename Ret, typename... Args>
struct func_impl_base {
  // note: no virtual dtor due to destroy()

  virtual func_impl_base* move(void* space) noexcept = 0;
  virtual void destroy() = 0;

  virtual Ret call(Args&&... args) = 0;
};

template<typename F, typename Ret, typename... Args>
class func_impl : public func_impl_base<Ret, Args...> {
  static constexpr bool alloc_on_heap = needs_heap<func_impl>;

  func_impl(F&& func) : func_(std::move(func)) {}

public:
  static func_impl* create(F&& func, void* space);

  func_impl_base<Ret, Args...>* move(void* space) noexcept override;
  void destroy() override;

  Ret call(Args&&... args) override;

private:
  F func_;
};

template<typename F, typename Ret, typename... Args>
func_impl<F, Ret, Args...>* func_impl<F, Ret, Args...>::create(F&& func, [[maybe_unused]] void* space) {
  if constexpr (alloc_on_heap) {
    return new func_impl(std::move(func));
  } else {
    new (space) func_impl(std::move(func));
  }
}

template<typename F, typename Ret, typename... Args>
func_impl_base<Ret, Args...>* func_impl<F, Ret, Args...>::move(void* space) noexcept {
  ASSERT(!alloc_on_heap) << "Attempting to move heap-allocated function into local space";
  return new (space) func_impl(std::move(func_));
}

template<typename F, typename Ret, typename... Args>
void func_impl<F, Ret, Args...>::destroy() {
  if constexpr (alloc_on_heap) {
    delete this;
  } else {
    this->~func_impl();
  }
}

template<typename F, typename Ret, typename... Args>
Ret func_impl<F, Ret, Args...>::call(Args&&... args) {
  if constexpr (std::is_void_v<Ret>) {
    std::invoke(func_, std::forward<Args>(args)...);  // discard return value (if any)
  } else {
    return std::invoke(func_, std::forward<Args>(args)...);
  }
}


// nullness checks

template<typename T>
constexpr bool func_is_null(const T&) {
  return false;
}

template<typename T>
constexpr bool func_is_null(T* ptr) {
  return !ptr;
}

template<typename Class, typename Mem>
constexpr bool func_is_null(Mem Class::* ptr) {
  return !ptr;
}

}


template<typename Ret, typename... Args>
class function<Ret(Args...)> : public non_copyable {
  template<typename F>
  using enable_if_compatible = std::enable_if_t<
    std::is_invocable_r_v<Ret, F&, Args&&...>
    && !std::is_same_v<F, function>
  >;
  
public:
  constexpr function() = default;
  constexpr function(std::nullptr_t) : function() {}
  function(function&& rhs) noexcept;
  
  template<typename F, typename = enable_if_compatible<F>>
  function(F func);
  
  ~function() { reset(); }
  
  function& operator=(std::nullptr_t) { reset(); }
  function& operator=(function&& rhs) noexcept;
  
  template<typename F, typename = enable_if_compatible<F>>
  function& operator=(F func);
  
  void reset();
  void swap(function& other) noexcept;
  
  explicit operator bool() const { return !!impl_; }
  Ret operator()(Args... args) const;
  
private:
  void* get_space() { return &space_; }
  bool is_local() const { return impl_ == get_space(); }

  template<typename F>
  void set_from(F&& func);
  void move_from(function&& rhs) noexcept;

  impl::func_space space_;
  impl::func_impl_base<Ret, Args...>* impl_ = nullptr;
};


template<typename Ret, typename... Args>
function<Ret(Args...)>::function(function&& rhs) noexcept {
  move_from(std::move(rhs));
}

template<typename Ret, typename... Args>
template<typename F, typename>
function<Ret(Args...)>::function(F func) {
  set_from(std::move(func));
}

template<typename Ret, typename... Args>
function<Ret(Args...)>& function<Ret(Args...)>::operator=(function&& rhs) noexcept {
  if (this == &rhs) {
    return *this;
  }

  reset();
  move_from(std::move(rhs));
  return *this;
}

template<typename Ret, typename... Args>
template<typename F, typename>
function<Ret(Args...)>& function<Ret(Args...)>::operator=(F func) {
  reset();
  set_from(std::move(func));
}


template<typename Ret, typename... Args>
void function<Ret(Args...)>::reset() {
  if (impl_) {
    impl_->destroy();
    impl_ = nullptr;
  }
}

template<typename Ret, typename... Args>
void function<Ret(Args...)>::swap(function& other) noexcept {
  if (!is_local() && !other.is_local()) {
    std::swap(impl_, other.impl_);  // both on heap - swap pointers
  } else {
    function tmp = std::move(other);  // three-way move
    other = std::move(*this);
    *this = std::move(tmp);
  }
}


template<typename Ret, typename... Args>
Ret function<Ret(Args...)>::operator()(Args... args) const {
  if (!impl_) {
    throw std::bad_function_call();
  }
  return impl_->call(std::forward<Args>(args)...);
}


// PRIVATE

template<typename Ret, typename... Args>
template<typename F>
void function<Ret(Args...)>::set_from(F&& func) {
  ASSERT(!impl_) << "This assumes that the function is empty";

  if (impl::func_is_null(func)) {
    return;  // already empty
  }

  impl_ = impl::func_impl<F, Ret, Args...>::create(std::move(func), get_space());
}

template<typename Ret, typename... Args>
void function<Ret(Args...)>::move_from(function&& rhs) noexcept {
  ASSERT(!impl_) << "This assumes that the function is empty";

  if (!rhs) {
    return;  // already empty
  }

  if (rhs.is_local()) {
    impl_ = rhs.impl_->move(get_space());  // stored locally - move target
  } else {
    impl_ = rhs.impl_;  // stored on heap - steal pointer
    rhs.impl_ = nullptr;
  }
}


// NONMEMBER

template<typename R, typename... Args>
inline void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs) noexcept {
  lhs.swap(rhs);
}


template<typename R, typename... Args>
bool operator==(const function<R(Args...)>& func, std::nullptr_t) {
  return !func;
}

template<typename R, typename... Args>
bool operator==(std::nullptr_t, const function<R(Args...)>& func) {
  return !func;
}

template<typename R, typename... Args>
bool operator!=(const function<R(Args...)>& func, std::nullptr_t) {
  return !!func;
}

template<typename R, typename... Args>
bool operator!=(std::nullptr_t, const function<R(Args...)>& func) {
  return !!func;
}

}  // namespace base