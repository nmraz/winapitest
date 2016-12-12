#pragma once

#include <shared_mutex>

namespace base {

class TaskRunner;

namespace impl {

struct TaskRunnerRef {
	TaskRunnerRef(TaskRunner* runner) 
		: runner(runner) {}

	std::shared_mutex lock;  // prevents the runner from destructing during task posting
	TaskRunner* runner;
};

}  // namespace impl
}  // namespace base