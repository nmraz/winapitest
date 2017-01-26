#pragma once

#include "base/assert.h"
#include "base/AutoRestore.h"
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

	SlotHandle on(Slot slot);
	void operator()(Args... args) const;

private:
	friend SlotHandle;
	// list is required here because it guarantees pointer validity even after
	// mutation of unrelated entries (SlotHandle holds a pointer)
	using Slots = std::list<Slot>;
	using SlotIter = typename Slots::const_iterator;

	void removeSlot(void* slot);

	Slots mSlots;
	mutable SlotIter* mNextSlot = nullptr;  // points to the slot after the currently executing slot
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

	AutoRestore<SlotIter*> restore(mNextSlot, &it);
	while (it != mSlots.end()) {
		(*it++)(args...);
	}
}


// PRIVATE

template<typename... Args>
void Signal<Args...>::removeSlot(void* slotAddr) {
	auto it = std::find_if(mSlots.begin(), mSlots.end(),
		[slotAddr](const auto& slot) {
			return &slot == slotAddr;
		}
	);
	ASSERT(it != mSlots.end());

	if (mNextSlot && it == *mNextSlot) {
		++(*mNextSlot);  // increment before it is invalidated by erase
	}
	mSlots.erase(it);
}

}  // namepsace base