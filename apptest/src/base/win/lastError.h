#pragma once

#include <system_error>

namespace base {
namespace win {

[[noreturn]] void throwLastError(const char* what = "");
std::error_code lastErrorCode();

}  // namespace win
}  // namespace base
