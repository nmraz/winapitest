#include "IoEventLoop.h"

#include "base/eventLoop/TaskRunner.h"
#include "base/win/lastError.h"
#include <Windows.h>

namespace base {

IoEventLoop::IoEventLoop()
	: mWakeUpEvt(::CreateEventW(nullptr, false, false, nullptr)) {
	if (!mWakeUpEvt) {
		win::throwLastError("Failed to create event");
	}
}


void IoEventLoop::sleep(const std::optional<Task::Delay>& delay) {
	DWORD waitTime = INFINITE;
	if (delay) {
		auto millis = std::chrono::ceil<std::chrono::milliseconds>(*delay);
		waitTime = static_cast<DWORD>(millis.count());
	}

	::WaitForSingleObjectEx(mWakeUpEvt.get(), waitTime, true);
}

void IoEventLoop::wakeUp() {
	::SetEvent(mWakeUpEvt.get());
}

}  // namepace base