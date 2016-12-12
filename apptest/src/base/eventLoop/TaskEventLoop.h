#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/thread/Waitable.h"

namespace base {

class TaskEventLoop : public EventLoop {
public:
	void doRun(TaskRunner& runner) override;
	void quit() override;

	void wakeUp() override;

private:
	bool mShouldQuit;

	Waitable mWakeUpEvt;
};

}