#include "EventLoop.h"

#include "base/assert.h"
#include "base/AutoRestore.h"
#include "base/eventLoop/TaskRunner.h"

namespace base {
namespace {

thread_local EventLoop* gCurrentLoop = nullptr;
thread_local int gNestingLevel = 0;

}  // namespace


struct EventLoop::LoopPusher {
	LoopPusher(EventLoop* loop);
	~LoopPusher();

	EventLoop* mPrevLoop;
};

EventLoop::LoopPusher::LoopPusher(EventLoop* loop)
	: mPrevLoop(gCurrentLoop) {
	gCurrentLoop = loop;
	TaskRunner::current().setLoop(loop);
}

EventLoop::LoopPusher::~LoopPusher() {
	gCurrentLoop = mPrevLoop;
	TaskRunner::current().setLoop(mPrevLoop);
}


void EventLoop::run() {
	LoopPusher push(this);
	AutoRestore<int> restoreNesting(gNestingLevel);

	++gNestingLevel;
	mShouldQuit = false;

	TaskRunner& runner = TaskRunner::current();

	while (true) {
		bool ranTask = runner.runPendingTask();
		if (mShouldQuit) {
			break;
		}

		ranTask |= runner.runDelayedTask();
		if (mShouldQuit) {
			break;
		}

		ranTask |= doWork();
		if (mShouldQuit) {
			break;
		}

		if (!ranTask) {
			sleep(runner.nextDelay());
		}
	}
}

void EventLoop::quit() {
	mShouldQuit = true;
}


bool EventLoop::doWork() {
	return false;
}


// static
EventLoop& EventLoop::current() {
	ASSERT(gCurrentLoop) << "No event loop running on this thread";
	return *gCurrentLoop;
}

// static
bool EventLoop::isNested() {
	return gNestingLevel > 1;
}


// PROTECTED

bool EventLoop::runPendingTask() {
	return TaskRunner::current().runPendingTask();
}

bool EventLoop::runDelayedTask() {
	return TaskRunner::current().runDelayedTask();
}

std::optional<Task::Delay> EventLoop::nextDelay() {
	return TaskRunner::current().nextDelay();
}

}  // namespace base