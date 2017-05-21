#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/win/ScopedHandle.h"

namespace base {

class IoEventLoop : public EventLoop {
public:
	IoEventLoop();

	void sleep(const std::optional<Task::Delay>& delay) override;
	void wakeUp() override;

private:
	win::ScopedHandle mWakeUpEvt;
};

}  // namespace base