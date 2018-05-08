#pragma once

#include "base/non_copyable.h"
#include <type_traits>
#include <utility>

namespace base {

template<typename F>
class final_act : public non_copy_movable {
public:
  explicit constexpr final_act(F callback);
  ~final_act();

private:
  F func_;
};

template<typename F>
constexpr final_act<F>::final_act(F func)
  : func_(std::move(func)) {
}

template<typename F>
final_act<F>::~final_act() {
  func_();
}

}  // namespace base