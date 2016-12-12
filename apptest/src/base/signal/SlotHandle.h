#pragma once

#include "base/NonCopyable.h"

namespace base {

class SlotHandle : public NonCopyable {
public:
	SlotHandle();

	template<typename Signal>
	SlotHandle(Signal* signal, int id);

	SlotHandle(SlotHandle&& rhs) noexcept;

	void off();

	SlotHandle& operator=(SlotHandle&& rhs);

private:
	void* mSignal;
	int mSlotId;
	void (*mRemover)(void*, int);

	template<typename... Args>
	friend class Signal;
};


template<typename Signal>
SlotHandle::SlotHandle(Signal* signal, int id)
	: mSignal(signal)
	, mSlotId(id)
	, mRemover([](void* signal, int slotId) { static_cast<Signal*>(signal)->removeSlot(slotId); }) {
}

}  // namespace base