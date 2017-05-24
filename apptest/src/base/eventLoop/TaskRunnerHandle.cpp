#include "TaskRunnerHandle.h"

#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskRunnerRef.h"
#include <utility>

namespace base {

BadTaskRunnerHandle::BadTaskRunnerHandle()
	: std::runtime_error("Bad TaskRunnerHandle") {
}


void TaskRunnerHandle::postTask(Task::Callback callback) {
	if (!mRef) {
		throw BadTaskRunnerHandle();
	}

	std::lock_guard<std::mutex> hold(mRef->lock);
	TaskRunner* runner = mRef->runner;
	
	if (!runner) {
		throw BadTaskRunnerHandle();
	}

	runner->postTask(std::move(callback));
}


// PRIVATE

TaskRunnerHandle::TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref)
	: mRef(std::move(ref)) {
}

// static
TaskRunnerHandle TaskRunnerHandle::currentHande() {
	return TaskRunner::current().handle();
}

}  // namespace base