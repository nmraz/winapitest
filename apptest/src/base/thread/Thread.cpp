#include "Thread.h"

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunner.h"
#include <utility>


namespace base {

Thread::Thread(std::unique_ptr<EventLoop> loop)
	: mThread(&Thread::run, this, std::move(loop)) {
}

Thread::Thread(LoopFactory factory)
	: mThread(&Thread::runWithFactory, this, std::move(factory)) {
}

Thread::~Thread() {
	stop();
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

void Thread::runWithFactory(LoopFactory factory) {
	run(factory());
}

void Thread::quit() {
	TaskRunner::current().postQuit();
}


void Thread::setTaskRunnerHandle(TaskRunnerHandle runner) {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	mRunner = std::move(runner);
}

}  // namepsace base