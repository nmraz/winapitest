#pragma once

#include "base/NonCopyable.h"
#include "base/eventLoop/Task.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

namespace base {

class EventLoop;

namespace impl {
struct TaskRunnerRef;
}


class TaskRunner : public NonCopyMovable {
	friend EventLoop;

	TaskRunner();
	~TaskRunner();

public:
	void postTask(Task::Callback callback, const Task::Delay& delay = Task::Delay::zero());
	void quitNow();
	void postQuit();

	TaskRunnerHandle handle();

	static TaskRunner& current();

private:
	using TaskQueue = std::queue<Task>;
	using DelayedTaskQueue = std::priority_queue<Task>;

	void setLoop(EventLoop* loop);

	bool runPendingTask();
	bool runDelayedTask();
	std::optional<Task::Delay> nextDelay() const;

	TaskQueue mTaskQueue;
	std::mutex mTaskLock;  // protects mTaskQueue, mCurrentLoop

	TaskQueue mCurrentTasks;  // to avoid locking the mutex every time, process tasks in batches
	DelayedTaskQueue mDelayedTasks;

	Task::RunTime mCachedNow;  // make running more efficient when multiple tasks have to run now

	EventLoop* mCurrentLoop;

	std::shared_ptr<impl::TaskRunnerRef> mHandleRef;  // synchronizes with handles on other threads
};

}  // namespace base