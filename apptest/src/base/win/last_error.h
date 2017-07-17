#pragma once

#include <system_error>

namespace base::win {

[[noreturn]] void throw_last_error(const char* what = "");
std::error_code last_error_code();

}  // namespace base::win
