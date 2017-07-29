#pragma once

#include <system_error>
#include <winerror.h>

namespace base::win {

std::error_code last_error_code();

[[noreturn]] void throw_last_error(const char* what = "");
void throw_if_failed(HRESULT hr, const char* what = "");

}  // namespace base::win
