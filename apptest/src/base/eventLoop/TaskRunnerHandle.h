#pragma once

#include "base/eventLoop/Task.h"
#include <memory>

namespace base {

class TaskRunner;

namespace impl {
struct TaskRunnerRef;
}

class TaskRunnerHandle {
	friend TaskRunner;

public:
	TaskRunnerHandle() = default;

	bool tryPostTask(Task::Callback callback, const Task::Delay& delay = Task::Delay::zero());
	bool tryPostTaskWithResponse(Task::Callback callback, Task::Callback response);

	void postTask(Task::Callback callback, const Task::Delay& delay = Task::Delay::zero());
	void postTaskWithResponse(Task::Callback callback, Task::Callback response);

private:
	TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref);

	std::shared_ptr<impl::TaskRunnerRef> mRef;
};

}  // namespace base