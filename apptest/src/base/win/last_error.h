#pragma once

#include <system_error>

namespace base {
namespace win {

[[noreturn]] void throw_last_error(const char* what = "");
std::error_code last_error_code();

}  // namespace win
}  // namespace base
