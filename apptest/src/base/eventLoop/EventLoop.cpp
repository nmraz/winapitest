#include "EventLoop.h"

#include "base/assert.h"
#include "base/eventLoop/TaskRunner.h"

namespace base {
namespace {

thread_local struct LoopContext {
	EventLoop* currentLoop;
	int nestingLevel = 0;
} gLoopCtx;

}  // namespace


struct EventLoop::LoopPusher {
	LoopPusher(EventLoop* loop);
	~LoopPusher();

	EventLoop* mPrevLoop;
};

EventLoop::LoopPusher::LoopPusher(EventLoop* loop)
	: mPrevLoop(gLoopCtx.currentLoop) {
	gLoopCtx.currentLoop = loop;
	TaskRunner::current().setLoop(loop);

	++gLoopCtx.nestingLevel;
}

EventLoop::LoopPusher::~LoopPusher() {
	gLoopCtx.currentLoop = mPrevLoop;
	TaskRunner::current().setLoop(mPrevLoop);

	--gLoopCtx.nestingLevel;
}


void EventLoop::run() {
	LoopPusher push(this);
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
	ASSERT(gLoopCtx.currentLoop) << "No event loop running on this thread";
	return *gLoopCtx.currentLoop;
}

// static
bool EventLoop::isNested() {
	return gLoopCtx.nestingLevel > 1;
}

}  // namespace base