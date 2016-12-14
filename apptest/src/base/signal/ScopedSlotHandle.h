#pragma once

#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"

namespace base {

class ScopedSlotHandle : public NonCopyable {
public:
	ScopedSlotHandle() = default;
	ScopedSlotHandle(SlotHandle handle);

	~ScopedSlotHandle();

	ScopedSlotHandle& operator=(ScopedSlotHandle rhs);
	ScopedSlotHandle& operator=(SlotHandle handle);

	void off();
	SlotHandle release();

private:
	SlotHandle mSlot;
};

}  // namespace base
