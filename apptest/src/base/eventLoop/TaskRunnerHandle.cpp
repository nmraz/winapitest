#include "TaskRunnerHandle.h"

#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskRunnerRef.h"
#include <utility>

namespace base {

bool TaskRunnerHandle::postTask(Task::Callback callback, const Task::Delay& delay) {
	if (!mRef) {
		return false;
	}

	std::shared_lock<std::shared_mutex> hold(mRef->lock);
	TaskRunner* runner = mRef->runner;
	
	if (!runner) {
		return false;
	}

	runner->postTask(std::move(callback), delay);
	return true;
}

bool TaskRunnerHandle::postTaskWithResponse(Task::Callback callback, Task::Callback response) {
	TaskRunnerHandle callerHandle = TaskRunner::current().handle();

	return postTask(
		[callback = std::move(callback), response = std::move(response), callerHandle = std::move(callerHandle)]() mutable {
			callback();
			callerHandle.postTask(std::move(response));
		}
	);
}


// PRIVATE

TaskRunnerHandle::TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref)
	: mRef(std::move(ref)) {}

}  // namespace base