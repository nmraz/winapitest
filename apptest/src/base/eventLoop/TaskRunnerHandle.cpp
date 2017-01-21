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

	std::shared_lock<std::shared_mutex> hold(mRef->lock);
	TaskRunner* runner = mRef->runner;
	
	if (!runner) {
		throw BadTaskRunnerHandle();
	}

	runner->postTask(std::move(callback));
}

void TaskRunnerHandle::postTaskAndThen(Task::Callback callback, Task::Callback then) {
	postTask([callback = std::move(callback), then = std::move(then),
		callerHandle = TaskRunner::current().handle()]() mutable {
			callback();
			callerHandle.postTask(std::move(then));
		}
	);
}

// PRIVATE

TaskRunnerHandle::TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref)
	: mRef(std::move(ref)) {
}

}  // namespace base