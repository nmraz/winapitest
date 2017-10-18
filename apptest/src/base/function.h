#pragma once

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

constexpr int func_space_ptr_size = 6;

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

  virtual func_impl_base* move(void* space) = 0;
  virtual void destroy() = 0;

  virtual Ret call(Args... args) = 0;
};

template<typename F, typename Ret, typename... Args>
class func_impl : public func_impl_base<Ret, Args...> {
  static constexpr bool alloc_on_heap = needs_heap<func_impl>;

  func_impl(F&& func) : func_(std::move(func)) {}

public:
  static func_impl* create(F&& func, void* space);

  func_impl_base<Ret, Args...>* move(void* space) override;
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
func_impl_base<Ret, Args...>* func_impl<F, Ret, Args...>::move(void* space) {
  return create(std::move(func_));
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
    std::invoke(func_, std::move(args)...);  // discard return value (if any)
  } else {
    return std::invoke(func_, std::move(args)...);
  }
}

}

}  // namespace base