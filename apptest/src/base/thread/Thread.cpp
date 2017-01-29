#include "Thread.h"

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunner.h"
#include <utility>


namespace base {
namespace {

// thrown when a thread should quit
// This class explicitly doesn't derive from std::exception to prevent it from
// inadvertently being caught in user code.
struct QuitNow {
};

}

Thread::Thread(LoopFactory factory)
	: mThread(&Thread::run, this, std::move(factory)) {
}

Thread::~Thread() {
	stop();
}


void Thread::stop(bool wait) {
	if (mThread.joinable()) {
		try {
			taskRunner().postTask(std::bind(&Thread::quit, this));
		} catch (const BadTaskRunnerHandle&) {
			// the thread is no loner running anyway
		}
		wait ? mThread.join() : mThread.detach();
	}
}


TaskRunnerHandle Thread::taskRunner() const {
	std::unique_lock<std::mutex> hold(mRunnerLock);
	mRunnerCv.wait(hold, [this] { return mHasRunner; });  // wait until the runner is created
	return mRunner;
}


// PRIVATE

void Thread::run(LoopFactory factory) {
	try {
		std::unique_ptr<EventLoop> loop = factory();
		TaskRunner runner;
		setTaskRunner(runner.handle());
		loop->run();
	} catch (const QuitNow&) {
	}
}

void Thread::quit() {
	throw QuitNow();
}


void Thread::setTaskRunner(TaskRunnerHandle runner) {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	mHasRunner = true;
	mRunner = std::move(runner);
	mRunnerCv.notify_all();
}

}  // namepsace base