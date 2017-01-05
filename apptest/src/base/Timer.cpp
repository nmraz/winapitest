#include "Timer.h"

#include "base/eventLoop/TaskRunner.h"
#include <functional>
#include <utility>

namespace base {

struct Timer::TimerTask {
	TimerTask(Timer* timer)
		: mTimer(timer) {}

	void cancel() { mTimer = nullptr; }
	void run();

	Timer* mTimer;
};

void Timer::TimerTask::run() {
	if (mTimer) {
		mTimer->fire();
	}
}


Timer::~Timer() {
	cancel();
}


void Timer::cancel() {
	if (isRunning()) {
		mCurrentTask->cancel();
		mCurrentTask = nullptr;
	}
}

bool Timer::isRunning() const {
	return mCurrentTask != nullptr;
}


SlotHandle Timer::onFire(FireSignal::Slot slot) {
	return mFireSignal.on(std::move(slot));
}


// PRIVATE

void Timer::doSet(const Task::Delay& interval, bool repeat) {
	cancel();

	mRepeating = repeat;
	mInterval = interval;
	mCurrentTask = std::make_shared<TimerTask>(this);

	repostTask();
}


void Timer::fire() {
	if (mRepeating) {
		repostTask();
	}

	mFireSignal();

	if (!mRepeating) {
		mCurrentTask = nullptr;  // no need to cancel() here, as the task will never run again
	}
}


void Timer::repostTask() {
	TaskRunner::current().postTask(std::bind(&TimerTask::run, mCurrentTask), mInterval);
}

}  // namespace base