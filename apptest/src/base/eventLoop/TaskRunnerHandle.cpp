#include "TaskRunnerHandle.h"

#include "base/assert.h"
#include "base/eventLoop/TaskRunner.h"
#include "base/eventLoop/TaskRunnerRef.h"
#include <utility>

namespace base {
namespace {

auto makeResponseCaller(Task::Callback callback, Task::Callback response) {
	return [callback = std::move(callback), response = std::move(response),
		callerHandle = TaskRunner::current().handle()]() mutable {
		callback();
		callerHandle.postTask(std::move(response));
	};
}

}  // namespace

bool TaskRunnerHandle::tryPostTask(Task::Callback callback, const Task::Delay& delay) {
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

bool TaskRunnerHandle::tryPostTaskAndThen(Task::Callback callback, Task::Callback then) {
	return tryPostTask(makeResponseCaller(std::move(callback), std::move(then)));
}


void TaskRunnerHandle::postTask(Task::Callback callback, const Task::Delay& delay) {
	[[maybe_unused]] bool posted = tryPostTask(std::move(callback), delay);
	ASSERT(posted) << "Can't post a task to an empty TaskRunnerHandle";
}

void TaskRunnerHandle::postTaskAndThen(Task::Callback callback, Task::Callback then) {
	postTask(makeResponseCaller(std::move(callback), std::move(then)));
}


// PRIVATE

TaskRunnerHandle::TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref)
	: mRef(std::move(ref)) {
}

}  // namespace base