#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/Task.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace base {

class TaskRunner;

class Thread {
public:
	using LoopFactory = std::function<std::unique_ptr<EventLoop>()>;

	Thread(std::unique_ptr<EventLoop> loop);
	Thread(LoopFactory factory);
	~Thread();

	void stop(bool wait = true);

	TaskRunnerHandle taskRunner() const;
	std::thread::id id() const { return mThread.get_id(); }

private:
	void run(std::unique_ptr<EventLoop> loop);
	void runWithFactory(LoopFactory factory);
	void quit();

	void setTaskRunnerHandle(TaskRunnerHandle runner);

	std::thread mThread;
	TaskRunnerHandle mRunner;
	mutable std::mutex mRunnerLock;  // protects mRunner
};

}  // namespace base