#include "Task.h"

namespace base {

Task::Task(Callback callback, const RunTime& runTime)
	: callback(callback)
	, runTime(runTime) {}

bool Task::operator<(const Task& rhs) const {
	return runTime > rhs.runTime;
}

}  // namespace base