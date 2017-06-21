#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace base {

class thread {
public:
	using loop_factory = std::function<std::unique_ptr<event_loop>()>;

	thread(loop_factory factory);
	thread(loop_factory factory, std::string name);
	~thread();

	void stop(bool wait = true);

	task_runner_handle task_runner() const;
	std::thread::id get_id() const { return thread_.get_id(); }

private:
	void run(loop_factory factory);
	void named_run(loop_factory factory, std::string name);

	void set_task_runner(task_runner_handle runner);

	// wait mechanism for task_runner
	mutable std::mutex runner_lock_;
	mutable std::condition_variable runner_cv_;
	bool has_runner_ = false;

	task_runner_handle runner_;
	std::thread thread_;  // the thread must be constructed (and started) last!
};

}  // namespace base