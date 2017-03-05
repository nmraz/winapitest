#pragma once

#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"

namespace base {

class ScopedSlotHandle : public NonCopyable, private SlotHandle {
public:
	using SlotHandle::off;
	using SlotHandle::active;
	using SlotHandle::block;
	using SlotHandle::blocked;
	using SlotHandle::operator=;

	ScopedSlotHandle() = default;
	ScopedSlotHandle(const SlotHandle& rhs);
	ScopedSlotHandle(SlotHandle&& rhs);
	~ScopedSlotHandle() { off(); }

	SlotHandle release() { return std::move(*this); }
};

}  // namespace base
