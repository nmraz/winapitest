#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/Task.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <memory>
#include <mutex>
#include <thread>

namespace base {

class TaskRunner;

class Thread {
public:
	Thread() = default;
	~Thread();

	void start(std::unique_ptr<EventLoop> loop);
	void stop(bool wait = true);

	TaskRunnerHandle taskRunner() const;

	std::thread::id id() const { return mThread.get_id(); }

private:
	void run(std::unique_ptr<EventLoop> loop);
	void quit();

	void setTaskRunnerHandle(TaskRunnerHandle runner);

	std::thread mThread;
	TaskRunnerHandle mRunner;
	mutable std::mutex mRunnerLock;  // protects mRunner
};

}  // namespace base