#pragma once

#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"
#include <algorithm>
#include <functional>
#include <list>
#include <utility>

namespace base {

template<typename... Args>
class Signal : public NonCopyMovable {
public:
	using Slot = std::function<void(Args...)>;

	SlotHandle on(Slot fn);
	void operator()(Args... args) const;

private:
	friend SlotHandle;
	void removeSlot(void* slot);

	std::list<Slot> mSlots;
};


template<typename... Args>
SlotHandle Signal<Args...>::on(Slot slot) {
	mSlots.push_back(std::move(slot));
	void* slotAddr = &*mSlots.rbegin();  // iterator => pointer

	return {this, slotAddr};
}


template<typename... Args>
void Signal<Args...>::operator()(Args... args) const {
	auto it = mSlots.begin();

	while (it != mSlots.end()) {
		(*it++)(args...);  // it must be incremented *before* the slot is called
	}
}


// PRIVATE

template<typename... Args>
void Signal<Args...>::removeSlot(void* slotAddr) {
	mSlots.erase(std::find_if(mSlots.begin(), mSlots.end(),
		[slotAddr](const auto& slot) {
			return &slot == slotAddr;
		})
	);
}

}  // namepsace base