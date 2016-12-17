#include "ScopedSlotHandle.h"
#include <utility>

namespace base {

ScopedSlotHandle::ScopedSlotHandle(SlotHandle handle)
	: mSlot(std::move(handle)) {
}

ScopedSlotHandle::~ScopedSlotHandle() {
	mSlot.off();
}

ScopedSlotHandle& ScopedSlotHandle::operator=(ScopedSlotHandle rhs) {
	return *this = std::move(rhs.mSlot);
}

ScopedSlotHandle& ScopedSlotHandle::operator=(SlotHandle handle) {
	off();
	mSlot = std::move(handle);

	return *this;
}

void ScopedSlotHandle::off() {
	mSlot.off();
}

SlotHandle ScopedSlotHandle::release() {
	return std::move(mSlot);
}

}  // namespace base