#pragma once

#include "base/eventLoop/EventLoop.h"
#include <condition_variable>
#include <mutex>

namespace base {

class TaskEventLoop : public EventLoop {
public:
	void sleep(const std::optional<Task::Delay>& delay) override;
	void wakeUp() override;

private:
	// wake-up mechanism
	bool mShouldWakeUp;
	std::mutex mWakeUpLock;
	std::condition_variable mWakeUpCv;
};

}