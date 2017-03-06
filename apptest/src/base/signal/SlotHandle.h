#pragma once

#include <memory>

namespace base {

namespace impl {
struct SlotRepBase;
}

class SlotHandle {
public:
	SlotHandle() = default;

	void off();
	void block(bool block = true);
	bool blocked() const;
	bool alive() const { return !mSlot.expired(); }

private:
	template<typename... Args>
	friend class Signal;

	explicit SlotHandle(std::weak_ptr<impl::SlotRepBase> slot)
		: mSlot(std::move(slot)) {
	}

	std::weak_ptr<impl::SlotRepBase> mSlot;
};

}  // namespace base