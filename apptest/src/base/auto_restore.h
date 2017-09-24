#pragma once

#include "base/non_copyable.h"
#include <utility>

namespace base {

template<typename T>
class auto_restore : public non_copy_movable {
public:
  auto_restore(T& var)
    : var_(&var)
    , old_val_(var) {
  }

  template<typename U>
  auto_restore(T& var, U&& new_val)
    : var_(&var)
    , old_val_(std::move(var)) {
    *var_ = std::forward<U>(new_val);
  }

  ~auto_restore() {
    *var_ = std::move(old_val_);
  }

private:
  T* var_;
  T old_val_;
};

}