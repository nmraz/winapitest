#include "IoEventLoop.h"

#include "base/eventLoop/TaskRunner.h"
#include "base/win/lastError.h"
#include <Windows.h>

namespace base {

io_event_loop::io_event_loop()
	: wake_up_evt_(::CreateEventW(nullptr, false, false, nullptr)) {
	if (!wake_up_evt_) {
		win::throw_last_error("Failed to create event");
	}
}


void io_event_loop::sleep(const std::optional<task::delay_type>& delay) {
	DWORD wait_time = INFINITE;
	if (delay) {
		auto millis = std::chrono::ceil<std::chrono::milliseconds>(*delay);
		wait_time = static_cast<DWORD>(millis.count());
	}

	::WaitForSingleObjectEx(wake_up_evt_.get(), wait_time, true);
}

void io_event_loop::wake_up() {
	::SetEvent(wake_up_evt_.get());
}

}  // namepace base