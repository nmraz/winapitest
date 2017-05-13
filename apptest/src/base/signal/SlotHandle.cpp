#include "SlotHandle.h"

#include "base/signal/Signal.h"

namespace base {

void SlotHandle::disconnect() {
	if (auto slot = mSlot.lock()) {
		slot->disconnect();
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