#include "ScopedSlotHandle.h"
#include <utility>

namespace base {

ScopedSlotHandle::ScopedSlotHandle(const SlotHandle& rhs)
	: SlotHandle(rhs) {
}

ScopedSlotHandle::ScopedSlotHandle(SlotHandle&& rhs)
	: SlotHandle(std::move(rhs)) {
}

}  // namespace base