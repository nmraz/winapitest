#include "SlotHandle.h"

namespace {

void noop(void*, void*) {}

}  // namespace


namespace base {

SlotHandle::SlotHandle()
	: mRemover(noop) {}

SlotHandle::SlotHandle(SlotHandle&& rhs) noexcept
	: mSignal(rhs.mSignal)
	, mSlot(rhs.mSlot)
	, mRemover(rhs.mRemover) {
	rhs.mRemover = noop;
}

void SlotHandle::off() {
	mRemover(mSignal, mSlot);
	mRemover = noop;
}

SlotHandle& SlotHandle::operator=(SlotHandle&& rhs) {
	mSignal = rhs.mSignal;
	mSlot = rhs.mSlot;
	mRemover = rhs.mRemover;

	rhs.mRemover = noop;

	return *this;
}

}  // namespace base