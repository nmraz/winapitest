#pragma once

#include "base/win/ScopedHandle.h"
#include <chrono>

namespace base {

class Waitable {
public:
	enum class ResetPolicy {
		automatic,
		manual
	};

	enum class WaitResult {
		wasSignaled,
		timedOut
	};


	Waitable(ResetPolicy resetPolicy = ResetPolicy::automatic, bool signaled = false);

	void signal();
	void reset();

	void wait();

	template<typename Rep, typename Period>
	WaitResult waitFor(const std::chrono::duration<Rep, Period>& waitTime) {
		return doWaitFor(std::chrono::ceil<std::chrono::milliseconds>(waitTime));
	}

private:
	WaitResult doWaitFor(const std::chrono::milliseconds& waitTime);

	win::ScopedHandle mNativeHandle;
};

}  // namespace base