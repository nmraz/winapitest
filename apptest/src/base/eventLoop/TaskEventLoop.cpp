#include "TaskEventLoop.h"

#include "base/eventLoop/TaskRunner.h"

namespace base {

void TaskEventLoop::sleep(const std::optional<Task::Delay>& delay) {
	std::unique_lock<std::mutex> hold(mWakeUpLock);

	if (delay) {
		mWakeUpCv.wait_for(hold, *delay, [this] { return mShouldWakeUp; });
	} else {
		mWakeUpCv.wait(hold, [this] { return mShouldWakeUp; });
	}
	mShouldWakeUp = false;
}

void TaskEventLoop::wakeUp() {
	std::lock_guard<std::mutex> hold(mWakeUpLock);
	mShouldWakeUp = true;
	mWakeUpCv.notify_one();
}

}  // namespace base