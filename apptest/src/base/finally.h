#pragma once

#include "base/non_copyable.h"
#include <type_traits>
#include <utility>

namespace base {

// TODO: make non-movable once VS supports class template argument deduction/guaranteed copy elision
template<typename F>
class final_act : public non_copyable {
public:
  explicit constexpr final_act(F callback);
  constexpr final_act(final_act&& rhs);

  ~final_act();

  void dismiss() { should_invoke_ = false; }

private:
  bool should_invoke_ = true;
  F func_;
};

template<typename F>
constexpr final_act<F>::final_act(F func)
  : func_(std::move(func)) {
}

template<typename F>
constexpr final_act<F>::final_act(final_act&& rhs)
  : should_invoke_(rhs.should_invoke_)
  , func_(std::move(rhs.func_)) {
  rhs.dismiss();
}

template<typename F>
final_act<F>::~final_act() {
  if (should_invoke_) {
    func_();
  }
}


// wrapper until class template argument deduction is supported
template<typename F>
constexpr auto finally(const F& func) {
  return final_act<F>(f);
}

template<typename F>
constexpr auto finally(F&& func) {
  return final_act<F>(std::forward<F>(func));
}

}  // namespace base