#pragma once

#include <string>

namespace base {

void setCurrentThreadName(std::string name);
const std::string& getCurrentThreadName();

}  // namespace base