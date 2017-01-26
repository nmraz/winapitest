#pragma once

#include "base/eventLoop/EventLoop.h"
#include <condition_variable>
#include <mutex>

namespace base {

class TaskEventLoop : public EventLoop {
public:
	void doRun(TaskRunner& runner) override;
	void quit() override;

	void wakeUp() override;

private:
	bool mShouldQuit;

	// wake-up mechanism
	bool mShouldWakeUp;
	std::mutex mWakeUpLock;
	std::condition_variable mWakeUpCv;
};

}