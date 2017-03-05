#include "SlotHandle.h"

#include "base/signal/Signal.h"

namespace base {

void SlotHandle::off() {
	if (auto slot = mSlot.lock()) {
		slot->off();
	}
}

void SlotHandle::block(bool block) {
	if (auto slot = mSlot.lock()) {
		slot->block(block);
	}
}

bool SlotHandle::blocked() const {
	if (auto slot = mSlot.lock()) {
		return slot->blocked();
	}
	return true;
}

}  // namespace base