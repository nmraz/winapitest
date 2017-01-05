#pragma once

#include "base/NonCopyable.h"

namespace base {

class SlotHandle : public NonCopyable {
public:
	SlotHandle();
	SlotHandle(SlotHandle&& rhs) noexcept;

	void off();

	SlotHandle& operator=(SlotHandle&& rhs);

private:
	template<typename Signal>
	SlotHandle(Signal* signal, void* slot);

	void* mSignal;
	void* mSlot;
	void (*mRemover)(void*, void*);

	template<typename... Args>
	friend class Signal;
};


template<typename Signal>
SlotHandle::SlotHandle(Signal* signal, void* slot)
	: mSignal(signal)
	, mSlot(slot)
	, mRemover([](void* signal, void* slot) { static_cast<Signal*>(signal)->removeSlot(slot); }) {
}

}  // namespace base