#include "task.h"

namespace base {

task::task(callback_type callback, const run_time_type& run_time)
	: callback(callback)
	, run_time(run_time) {}

bool task::operator<(const task& rhs) const {
	return run_time > rhs.run_time;
}

}  // namespace base