#include "SlotHandle.h"

namespace {

void noop(void*, int) {}

}  // namespace


namespace base {

SlotHandle::SlotHandle()
	: mRemover(noop) {}

SlotHandle::SlotHandle(SlotHandle&& rhs) noexcept
	: mSignal(rhs.mSignal)
	, mSlotId(rhs.mSlotId)
	, mRemover(rhs.mRemover) {
	rhs.mRemover = noop;
}

void SlotHandle::off() {
	mRemover(mSignal, mSlotId);
	mRemover = noop;
}

SlotHandle& SlotHandle::operator=(SlotHandle&& rhs) {
	mSignal = rhs.mSignal;
	mSlotId = rhs.mSlotId;
	mRemover = rhs.mRemover;

	rhs.mRemover = noop;

	return *this;
}

}  // namespace base