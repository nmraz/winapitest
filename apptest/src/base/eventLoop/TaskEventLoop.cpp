#include "TaskEventLoop.h"

#include "base/eventLoop/TaskRunner.h"

namespace base {

void TaskEventLoop::doRun(TaskRunner& runner) {
	while (true) {
		bool ranTask = runner.runPendingTask();
		if (shouldQuit()) {
			break;
		}
		
		ranTask |= runner.runDelayedTask();
		if (shouldQuit()) {
			break;
		}

		if (ranTask) {
			continue;
		}

		// We finished running the current tasks:
		// wait until we have to process the next delayed task or the runner wakes us up explicitly
		std::optional<Task::Delay> delay = runner.nextDelay();
		std::unique_lock<std::mutex> hold(mWakeUpLock);

		if (delay) {
			mWakeUpCv.wait_for(hold, *delay, [this] { return mShouldWakeUp; });
		} else {
			mWakeUpCv.wait(hold, [this] { return mShouldWakeUp; });
		}
		mShouldWakeUp = false;
	}
}


void TaskEventLoop::wakeUp() {
	std::lock_guard<std::mutex> hold(mWakeUpLock);
	mShouldWakeUp = true;
	mWakeUpCv.notify_one();
}

}  // namespace base