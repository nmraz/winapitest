#pragma once

#include "base/NonCopyable.h"

namespace base {

class TaskRunner;

class EventLoop : public NonCopyMovable {
public:
	void run();
	void quit();

	virtual ~EventLoop() = default;
	virtual void doRun(TaskRunner& runner) = 0;
	virtual void wakeUp() = 0;

	static EventLoop& current();
	static bool isNested();

protected:
	bool shouldQuit() const;

private:
	struct LoopPusher;

	bool mShouldQuit;
};

}  // namespace base
