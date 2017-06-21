#pragma once

#include <mutex>

namespace base {

class task_runner;

namespace impl {

struct task_runner_ref {
	task_runner_ref(task_runner* runner) 
		: runner(runner) {}

	std::mutex lock;  // prevents the runner from destructing during task posting
	task_runner* runner;
};

}  // namespace impl
}  // namespace base