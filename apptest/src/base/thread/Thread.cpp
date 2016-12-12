#include "Thread.h"

#include "base/assert.h"
#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunner.h"
#include <functional>
#include <utility>


namespace base {

Thread::~Thread() {
	stop();
}


void Thread::start(std::unique_ptr<EventLoop> loop) {
	ASSERT(!mThread.joinable()) << "Thread already running";
	mThread = std::thread(&Thread::run, this, std::move(loop));
}

void Thread::stop(bool wait) {
	if (mThread.joinable()) {
		taskRunner().postTask(std::bind(&Thread::quit, this));
		wait ? mThread.join() : mThread.detach();
	}
}


TaskRunnerHandle Thread::taskRunner() const {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	return mRunner;
}


// PRIVATE

void Thread::run(std::unique_ptr<EventLoop> loop) {
	TaskRunner runner;
	setTaskRunnerHandle(runner.handle());
	loop->run();
}

void Thread::quit() {
	TaskRunner::current().postQuit();
}


void Thread::setTaskRunnerHandle(TaskRunnerHandle runner) {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	mRunner = std::move(runner);
}

}  // namepsace base