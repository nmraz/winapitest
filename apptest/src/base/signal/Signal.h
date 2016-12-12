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
	friend SlotHandle;

public:
	using Function = std::function<void(Args...)>;

	Signal();

	SlotHandle on(Function fn);

	void operator()(Args... args) const;

private:
	struct Slot {
		Slot(Function fn, int id);

		Function fn;
		int id;
	};

	void removeSlot(int id);

	std::list<Slot> mSlots;
	int mNextId;
};


template<typename... Args>
Signal<Args...>::Slot::Slot(Function fn, int id)
	: fn(std::move(fn))
	, id(id) {
}


template<typename... Args>
Signal<Args...>::Signal()
	: mNextId(0) {
}


template<typename... Args>
SlotHandle Signal<Args...>::on(Function fn) {
	int id = ++mNextId;
	mSlots.emplace_back(std::move(fn), id);
	return {this, id};
}


template<typename... Args>
void Signal<Args...>::operator()(Args... args) const {
	auto it = mSlots.begin();

	while (it != mSlots.end()) {
		(it++)->fn(args...);  // no forwarding to prevent some args from being moved
	}
}


// PRIVATE

template<typename... Args>
void Signal<Args...>::removeSlot(int id) {
	mSlots.erase(std::find_if(mSlots.begin(), mSlots.end(),
		[id](const auto& slot) {
			return slot.id == id;
		})
	);
}

}  // namepsace base