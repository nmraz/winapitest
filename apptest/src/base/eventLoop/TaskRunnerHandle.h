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
	bool tryPostTaskAndThen(Task::Callback callback, Task::Callback then);

	void postTask(Task::Callback callback, const Task::Delay& delay = Task::Delay::zero());
	void postTaskAndThen(Task::Callback callback, Task::Callback then);

private:
	TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref);

	std::shared_ptr<impl::TaskRunnerRef> mRef;
};

}  // namespace base