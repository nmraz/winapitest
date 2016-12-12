#include "ScopedSlotHandle.h"
#include <utility>

namespace base {

ScopedSlotHandle::ScopedSlotHandle(SlotHandle handle)
	: mSlot(std::move(handle)) {}

ScopedSlotHandle::~ScopedSlotHandle() {
	mSlot.off();
}

ScopedSlotHandle& ScopedSlotHandle::operator=(SlotHandle handle) {
	mSlot = std::move(handle);

	return *this;
}

void ScopedSlotHandle::off() {
	mSlot.off();
}

}  // namespace base