#pragma once

#include "base/eventLoop/Task.h"
#include "base/NonCopyable.h"
#include "base/signal/Signal.h"
#include <chrono>
#include <memory>

namespace base {

class Timer : public NonCopyMovable {
	using FireSignal = Signal<>;

public:
	~Timer();

	template<typename Rep, typename Period>
	void set(const std::chrono::duration<Rep, Period>& interval, bool repeat = false);
	void cancel();

	bool isRunning() const;

	SlotHandle onFire(FireSignal::Function fn);

private:
	struct TimerTask;

	void doSet(const Task::Delay& delay, bool repeat);
	void fire();
	void repostTask();

	bool mRepeating;
	Task::Delay mInterval;

	std::shared_ptr<TimerTask> mCurrentTask;
	FireSignal mFireSignal;
};

template<typename Rep, typename Period>
inline void Timer::set(const std::chrono::duration<Rep, Period>& interval, bool repeat) {
	doSet(std::chrono::ceil<Task::Delay>(interval), repeat);
}

}  // namespace base
