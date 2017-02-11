#pragma once

#include "base/eventLoop/EventLoop.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace base {

class Thread {
public:
	using LoopFactory = std::function<std::unique_ptr<EventLoop>()>;

	Thread(LoopFactory factory);
	~Thread();

	void stop(bool wait = true);

	TaskRunnerHandle taskRunner() const;
	std::thread::id id() const { return mThread.get_id(); }

private:
	void run(LoopFactory factory);
	void quit();

	void setTaskRunner(TaskRunnerHandle runner);

	// wait mechanism for taskRunner
	mutable std::mutex mRunnerLock;
	mutable std::condition_variable mRunnerCv;
	bool mHasRunner = false;

	TaskRunnerHandle mRunner;
	std::thread mThread;  // the thread must be constructed (and started) last!
};

}  // namespace base