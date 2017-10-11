#pragma once

#include <memory>

namespace base {

class task_runner;

std::shared_ptr<task_runner> get_inline_task_runner();

}  // namespace base