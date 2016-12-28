#include "TaskRunner.h"

#include "base/assert.h"
#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include "base/eventLoop/TaskRunnerRef.h"
#include <functional>
#include <utility>

namespace base {
namespace {

thread_local TaskRunner* gCurrentRunner = nullptr;

}  // namespace


TaskRunner::TaskRunner()
	: mCurrentLoop(nullptr)
	, mHandleRef(std::make_shared<impl::TaskRunnerRef>(this)) {
	gCurrentRunner = this;
}

TaskRunner::~TaskRunner() {
	std::lock_guard<std::shared_mutex> hold(mHandleRef->lock);
	mHandleRef->runner = nullptr;

	gCurrentRunner = nullptr;
}


void TaskRunner::postTask(Task::Callback callback, const Task::Delay& delay) {
	ASSERT(delay.count() >= 0) << "Can't post a task with a negative delay";
	ASSERT(callback) << "Can't post an empty callback";

	Task::RunTime runTime = delay.count() == 0 ? Task::RunTime() : Task::Clock::now() + delay;
	std::lock_guard<std::mutex> hold(mTaskLock);
	
	bool wasEmpty = mTaskQueue.empty();
	mTaskQueue.emplace(std::move(callback), runTime);

	if (mCurrentLoop && wasEmpty) {
		mCurrentLoop->wakeUp();
	}
}

void TaskRunner::postQuit() {
	postTask(std::bind(&TaskRunner::quitTask, this));
}


TaskRunnerHandle TaskRunner::handle() {
	return TaskRunnerHandle(mHandleRef);
}


bool TaskRunner::runPendingTask() {
	if (mCurrentTasks.empty()) {
		std::lock_guard<std::mutex> hold(mTaskLock);

		mTaskQueue.swap(mCurrentTasks);
	}

	while (!mCurrentTasks.empty()) {
		Task task = std::move(const_cast<Task&>(mCurrentTasks.front()));
		mCurrentTasks.pop();
		
		if (task.runTime == Task::RunTime()) {
			task.callback();

			return true;
		}

		mDelayedTasks.push(std::move(task));
	}

	return false;
}

bool TaskRunner::runDelayedTask() {
	if (mDelayedTasks.empty()) {
		return false;
	}

	const Task& task = mDelayedTasks.top();

	if (task.runTime > mCachedNow) {
		if (task.runTime > (mCachedNow = Task::Clock::now())) {
			return false;
		}
	}

	task.callback();
	mDelayedTasks.pop();

	return true;
}


std::optional<Task::Delay> TaskRunner::nextDelay() const {
	if (mDelayedTasks.empty()) {
		return std::nullopt;
	}
	
	Task::Delay delay = mDelayedTasks.top().runTime - Task::Clock::now();
	return delay < Task::Delay::zero() ? Task::Delay::zero() : delay;
}


// static
TaskRunner& TaskRunner::current() {
	ASSERT(gCurrentRunner) << "TaskRunner not initialized on this thread";

	return *gCurrentRunner;
}


// PRIVATE

void TaskRunner::setLoop(EventLoop* loop) {
	std::lock_guard<std::mutex> hold(mTaskLock);
	mCurrentLoop = loop;
}

void TaskRunner::quitTask() {
	if (EventLoop::isNested()) {
		postQuit();
	}

	EventLoop::current().quit();
}

}  // namespace base