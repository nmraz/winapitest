#include "Thread.h"

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunner.h"
#include <utility>


namespace base {

Thread::Thread(LoopFactory factory)
	: mThread(&Thread::run, this, std::move(factory)) {
}

Thread::~Thread() {
	stop();
}


void Thread::stop(bool wait) {
	taskRunner().postTask(std::bind(&Thread::quit, this));
	wait ? mThread.join() : mThread.detach();
}


TaskRunnerHandle Thread::taskRunner() const {
	std::unique_lock<std::mutex> hold(mRunnerLock);
	mRunnerCv.wait(hold, [this] {return mHasRunner; });  // wait until the runner is created
	return mRunner;
}


// PRIVATE

void Thread::run(LoopFactory factory) {
	std::unique_ptr<EventLoop> loop = factory();
	TaskRunner runner;
	setTaskRunner(runner.handle());
	loop->run();
}

void Thread::quit() {
	TaskRunner::current().postQuit();
}


void Thread::setTaskRunner(TaskRunnerHandle runner) {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	mHasRunner = true;
	mRunner = std::move(runner);
	mRunnerCv.notify_all();
}

}  // namepsace base