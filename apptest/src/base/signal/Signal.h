#pragma once

#include "base/assert.h"
#include "base/AutoRestore.h"
#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace base {
namespace impl {

struct SlotRepBase {
	virtual void off() = 0;
	virtual void block(bool block) = 0;
	virtual bool blocked() const = 0;
};

}  // namespace impl

template<typename... Args>
class Signal : public NonCopyMovable {
public:
	using Slot = std::function<void(Args...)>;

	SlotHandle on(Slot slot);
	void operator()(Args... args) const;

private:
	friend SlotHandle;

	struct SlotRep : impl::SlotRepBase {
		SlotRep(Signal* signal, Slot slot)
			: mSignal(signal)
			, mSlot(std::move(slot)) {
		}

		void off() override { mSignal->removeSlot(this); }
		void block(bool block) override { mBlocked = block; }
		bool blocked() const override { return mBlocked; }

		void call(Args&... args) const { if (!mBlocked) mSlot(args...); }

		Signal* mSignal;
		Slot mSlot;
		bool mBlocked = false;
	};

	using SlotPtr = std::shared_ptr<SlotRep>;
	using Slots = std::vector<SlotPtr>;

	void removeSlot(SlotRep* rep);
	void tidy();  // removes elements marked for removal, if safe

	Slots mSlots;
	// number of nested emissions - when 0, it is safe to remove elements directly
	mutable int mEmitDepth = 0;
};


template<typename... Args>
SlotHandle Signal<Args...>::on(Slot slot) {
	auto rep = std::make_shared<SlotRep>(this, std::move(slot));
	mSlots.push_back(rep);
	return SlotHandle(rep);
}


template<typename... Args>
void Signal<Args...>::operator()(Args... args) const {
	{
		AutoRestore<int> restore(mEmitDepth);
		++mEmitDepth;

		// iterators may be invalidated, use indices instead
		for (std::size_t i = 0; i < mSlots.size(); ++i) {
			const auto& slot = mSlots[i];
			if (slot) {
				slot->call(args...);
			}
		}
	}

	const_cast<Signal*>(this)->tidy();
}


// PRIVATE

template<typename... Args>
void Signal<Args...>::removeSlot(SlotRep* rep) {
	auto it = std::find_if(mSlots.begin(), mSlots.end(),
		[&](const auto& cur) {
			return cur.get() == rep;
		}
	);
	ASSERT(it != mSlots.end()) << "Corrupt signal state";

	if (!mEmitDepth) {
		mSlots.erase(it);
	} else {
		*it = nullptr;
	}
}

template<typename... Args>
void Signal<Args...>::tidy() {
	if (!mEmitDepth) {
		mSlots.erase(std::remove(mSlots.begin(), mSlots.end(), nullptr), mSlots.end());
	}
}

}  // namepsace base