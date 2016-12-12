#pragma once

#include <chrono>
#include <functional>

namespace base {

struct Task {
	using Callback = std::function<void()>;

	using Clock = std::chrono::steady_clock;
	using RunTime = Clock::time_point;
	using Delay = RunTime::duration;

	Task(Callback callback, const RunTime& runTime);
	bool operator<(const Task& rhs) const;

	Callback callback;
	RunTime runTime;
};

}  // namespace base
