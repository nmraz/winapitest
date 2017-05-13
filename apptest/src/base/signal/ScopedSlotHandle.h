#pragma once

#include "base/NonCopyable.h"
#include "base/signal/SlotHandle.h"

namespace base {

class ScopedSlotHandle : public NonCopyable, private SlotHandle {
public:
	using SlotHandle::disconnect;
	using SlotHandle::connected;
	using SlotHandle::block;
	using SlotHandle::blocked;
	using SlotHandle::operator=;

	ScopedSlotHandle() = default;
	ScopedSlotHandle(const SlotHandle& rhs);
	ScopedSlotHandle(SlotHandle&& rhs);
	~ScopedSlotHandle() { disconnect(); }

	SlotHandle release() { return std::move(*this); }
};

}  // namespace base
