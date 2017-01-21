#pragma once

#include "base/eventLoop/Task.h"
#include <memory>
#include <stdexcept>

namespace base {

struct BadTaskRunnerHandle : std::runtime_error {
	BadTaskRunnerHandle();
};


class TaskRunner;

namespace impl {
struct TaskRunnerRef;
}

class TaskRunnerHandle {
	friend TaskRunner;

public:
	TaskRunnerHandle() = default;

	void postTask(Task::Callback callback);
	void postTaskAndThen(Task::Callback callback, Task::Callback then);

private:
	TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref);

	std::shared_ptr<impl::TaskRunnerRef> mRef;
};

}  // namespace base