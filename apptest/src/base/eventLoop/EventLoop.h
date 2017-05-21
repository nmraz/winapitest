#pragma once

#include "base/eventLoop/Task.h"
#include "base/NonCopyable.h"
#include <optional>

namespace base {

class TaskRunner;

class EventLoop : public NonCopyMovable {
public:
	void run();
	void quit();

	virtual ~EventLoop() = default;

	virtual bool doWork();
	virtual void sleep(const std::optional<Task::Delay>& delay) = 0;
	virtual void wakeUp() = 0;

	static EventLoop& current();
	static bool isNested();

private:
	struct LoopPusher;

	bool mShouldQuit;
};

}  // namespace base
