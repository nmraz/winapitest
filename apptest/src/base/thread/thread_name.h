#pragma once

#include <string>
#include <thread>

namespace base {

void set_current_thread_name(std::string name);

std::string get_thread_name(std::thread::id id);
std::string get_current_thread_name();

}  // namespace base