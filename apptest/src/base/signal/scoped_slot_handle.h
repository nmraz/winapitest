#pragma once

#include "base/non_copyable.h"
#include "base/signal/slot_handle.h"

namespace base {

class scoped_slot_handle : public slot_handle, public non_copyable {
public:
  scoped_slot_handle() = default;
  scoped_slot_handle(const slot_handle& rhs);
  scoped_slot_handle(slot_handle&& rhs);
  ~scoped_slot_handle() { disconnect(); }

  scoped_slot_handle& operator=(const slot_handle& rhs);
  scoped_slot_handle& operator=(slot_handle&& rhs);

  slot_handle detatch() { return std::move(*this); }
};

}  // namespace base
