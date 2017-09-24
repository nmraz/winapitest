#pragma once

#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"

namespace base {

class scoped_slot_handle : public non_copyable, private slot_handle {
public:
  using slot_handle::disconnect;
  using slot_handle::connected;
  using slot_handle::block;
  using slot_handle::blocked;
  using slot_handle::operator=;

  scoped_slot_handle() = default;
  scoped_slot_handle(const slot_handle& rhs);
  scoped_slot_handle(slot_handle&& rhs);
  ~scoped_slot_handle() { disconnect(); }

  slot_handle release() { return std::move(*this); }
};

}  // namespace base
