#include "Waitable.h"

#include "base/assert.h"
#include "base/win/lastError.h"
#include <Windows.h>

namespace base {

Waitable::Waitable(ResetPolicy resetPolicy, bool signaled)
	: mNativeHandle(::CreateEventW(nullptr, resetPolicy == ResetPolicy::manual, signaled, nullptr)) {
	if (!mNativeHandle) {
		win::throwLastError("CreateEvent failed");
	}
}


void Waitable::signal() {
	::SetEvent(mNativeHandle.get());
}

void Waitable::reset() {
	::ResetEvent(mNativeHandle.get());
}


void Waitable::wait() {
	::WaitForSingleObject(mNativeHandle.get(), INFINITE);
}


// PRIVATE

Waitable::WaitResult Waitable::doWaitFor(const std::chrono::milliseconds& waitTime) {
	ASSERT(waitTime.count() >= 0) << "cannot wait for a negative delay";

	return ::WaitForSingleObject(mNativeHandle.get(), static_cast<DWORD>(waitTime.count())) == WAIT_OBJECT_0 ? 
		WaitResult::wasSignalled
		: WaitResult::timedOut;
}

}  // namespace base