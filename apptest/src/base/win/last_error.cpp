#include "last_error.h"

#include <Windows.h>

namespace base::win {

std::error_code last_error_code() {
  return std::error_code(::GetLastError(), std::system_category());
}

[[noreturn]] void throw_last_error(const char* what) {
  throw std::system_error(last_error_code(), what);
}

void throw_if_failed(HRESULT hr, const char* what) {
  if (!SUCCEEDED(hr)) {
    throw std::system_error(static_cast<int>(hr), std::system_category(),  what);
  }
}

}  // namespace base::win