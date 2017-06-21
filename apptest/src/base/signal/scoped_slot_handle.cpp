#include "scoped_slot_handle.h"
#include <utility>

namespace base {

scoped_slot_handle::scoped_slot_handle(const slot_handle& rhs)
	: slot_handle(rhs) {
}

scoped_slot_handle::scoped_slot_handle(slot_handle&& rhs)
	: slot_handle(std::move(rhs)) {
}

}  // namespace base