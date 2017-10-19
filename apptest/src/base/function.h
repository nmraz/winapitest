#pragma once

#include "base/assert.h"
#include "base/non_copyable.h"
#include <functional>
#include <new>
#include <type_traits>

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
  || !std::is_nothrow_move_constructible_v<Impl>  // can't guarentee noexcept move/swap
  || !std::is_nothrow_move_assignable_v<Impl>;


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
func_impl<F, Ret, Args...>* func_impl<F, Ret, Args>::create(F&& func, [[maybe_unused]] void* space) {
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
constexpr bool func_not_null(const T&) {
  return true;
}

template<typename T>
constexpr bool func_not_null(const T* ptr) {
  return !!ptr;
}

template<typename Class, typename Mem>
constexpr bool func_not_null(const Mem Class::* ptr) {
  return !!ptr;
}

}


template<typename Ret, typename... Args>
class function<Ret(Args...)> : public non_copyable {
  template<typename F>
  using enable_if_compatible = std::enable_if_t<
    std::is_invocable_r_v<Ret, F&, Args&&...>
    && !std::is_same_v<F, function>
  >
  
public:
  constexpr function() : impl_(nullptr) {}
  constexpr function(std::nullptr_t) : function() {}
  function(function&& rhs) noexcept;
  
  template<typename F, typename = enable_if_compatible<F>>
  function(F func);
  
  ~function();
  
  function& operator=(std::nullptr_t);
  function& operator=(function&& rhs) noexcept;
  
  template<typename F typename = enable_if_compatible<F>>
  function& operator=(F func);
  
  void reset();
  void swap(function& other) noexcept;
  
  operator bool() const { return !!impl_; }
  Ret operator()(Args... args) const;
  
private:
  impl::func_space space_;
  impl::func_impl_base<Ret, Args...>* impl_;
}

}  // namespace base