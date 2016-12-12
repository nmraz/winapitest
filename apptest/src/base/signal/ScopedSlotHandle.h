#pragma once

#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"

namespace base {

class ScopedSlotHandle : public NonCopyable {
public:
	ScopedSlotHandle() = default;
	ScopedSlotHandle(SlotHandle handle);

	~ScopedSlotHandle();

	ScopedSlotHandle& operator=(SlotHandle handle);

	void off();

private:
	SlotHandle mSlot;
};

}  // namespace base
