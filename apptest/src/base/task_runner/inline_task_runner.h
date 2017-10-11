#pragma once

#include "base/task_runner/task_runner.h"
#include <memory>

namespace base {

std::shared_ptr<task_runner> get_inline_task_runner();

}  // namespace base