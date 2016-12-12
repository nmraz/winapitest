#pragma once

#include "base/NonCopyable.h"

namespace base {

class TaskRunner;

class EventLoop : public NonCopyMovable {
public:
	void run();

	virtual ~EventLoop() = default;

	virtual void doRun(TaskRunner& runner) = 0;
	virtual void quit() = 0;

	virtual void wakeUp() = 0;

	static EventLoop& current();
	static bool isNested();

private:
	struct LoopPusher;
};

}  // namespace base
