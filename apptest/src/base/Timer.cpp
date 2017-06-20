#include "Timer.h"

#include "base/eventLoop/TaskRunner.h"
#include <utility>

namespace base {

struct Timer::PostedTask {
	PostedTask(Timer* timer)
		: mTimer(timer) {}

	void cancel() { mTimer = nullptr; }
	void run();

	Timer* mTimer;
};

void Timer::PostedTask::run() {
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
	return mFireSignal.connect(std::move(slot));
}


// PRIVATE

void Timer::doSet(const Task::Delay& interval, bool repeat) {
	cancel();

	mRepeating = repeat;
	mInterval = interval;
	mCurrentTask = std::make_shared<PostedTask>(this);

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
	TaskRunner::current().postTask([this] { mCurrentTask->run(); }, mInterval);
}

}  // namespace base