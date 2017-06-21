#include "slot_handle.h"

#include "base/signal/Signal.h"

namespace base {

void slot_handle::disconnect() {
	if (auto slot = slot_.lock()) {
		slot->disconnect();
	}
}

void slot_handle::block(bool block) {
	if (auto slot = slot_.lock()) {
		slot->block(block);
	}
}

bool slot_handle::blocked() const {
	if (auto slot = slot_.lock()) {
		return slot->blocked();
	}
	return true;
}

}  // namespace base