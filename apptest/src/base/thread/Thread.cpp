#include "Thread.h"

#include "base/assert.h"
#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunner.h"
#include "base/thread/threadName.h"
#include <utility>

namespace base {
namespace {

thread_local bool quitProperly = false;

}  // namespace

Thread::Thread(LoopFactory factory)
	: mThread(&Thread::run, this, std::move(factory)) {
}

Thread::Thread(LoopFactory factory, std::string name)
	: mThread(&Thread::namedRun, this, std::move(factory), std::move(name)) {
}

Thread::~Thread() {
	stop();
}


void Thread::stop(bool wait) {
	if (mThread.joinable()) {
		taskRunner().postTask([] {
			quitProperly = true;
			TaskRunner::current().quitNow();
		});
		wait ? mThread.join() : mThread.detach();
	}
}


TaskRunnerHandle Thread::taskRunner() const {
	std::unique_lock<std::mutex> hold(mRunnerLock);
	mRunnerCv.wait(hold, [this] { return mHasRunner; });  // wait until the runner exists
	return mRunner;
}


// PRIVATE

void Thread::run(LoopFactory factory) {
	std::unique_ptr<EventLoop> loop = factory();
	setTaskRunner(TaskRunner::current().handle());
	loop->run();
	ASSERT(quitProperly) << "Thread should not quit of its own accord";
}

void Thread::namedRun(LoopFactory factory, std::string name) {
	setCurrentThreadName(std::move(name));
	run(std::move(factory));
}


void Thread::setTaskRunner(TaskRunnerHandle runner) {
	std::lock_guard<std::mutex> hold(mRunnerLock);
	mHasRunner = true;
	mRunner = std::move(runner);
	mRunnerCv.notify_all();
}

}  // namepsace base