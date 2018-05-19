#include "last_error.h"

#include <Windows.h>

namespace base::win {

std::error_code last_error() {
  return std::error_code(::GetLastError(), std::system_category());
}

[[noreturn]] void throw_last_error(const char* what) {
  auto code = last_error();
  if (code.value() == ERROR_OUTOFMEMORY) {
    throw std::bad_alloc();
  }

  throw std::system_error(code, what);
}

void throw_if_failed(HRESULT hr, const char* what) {
  if (!SUCCEEDED(hr)) {
    if (hr == E_OUTOFMEMORY) {
      throw std::bad_alloc();
    }

    throw std::system_error(static_cast<int>(hr), std::system_category(),  what);
  }
}

}  // namespace base::win