#include "scoped_slot_handle.h"
#include <utility>

namespace base {

scoped_slot_handle::scoped_slot_handle(const slot_handle& rhs)
  : slot_handle(rhs) {
}

scoped_slot_handle::scoped_slot_handle(slot_handle&& rhs)
  : slot_handle(std::move(rhs)) {
}


scoped_slot_handle& scoped_slot_handle::operator=(const slot_handle& rhs) {
  disconnect();
  static_cast<slot_handle&>(*this) = rhs;
  return *this;
}

scoped_slot_handle& scoped_slot_handle::operator=(slot_handle&& rhs) {
  disconnect();
  static_cast<slot_handle&>(*this) = std::move(rhs);
  return *this;
}

}  // namespace base