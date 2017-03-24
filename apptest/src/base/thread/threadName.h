#pragma once

#include <string>

namespace base {

void setCurrentThreadName(std::string name);
std::string getCurrentThreadName();

}  // namespace base