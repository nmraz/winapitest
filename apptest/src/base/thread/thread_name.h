#pragma once

#include <string>

namespace base {

void set_current_thread_name(std::string name);
const std::string& get_current_thread_name();

}  // namespace base