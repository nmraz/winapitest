#include "last_error.h"

#include <Windows.h>

namespace base::win {

[[noreturn]] void throw_last_error(const char* what) {
	throw std::system_error(last_error_code(), what);
}

std::error_code last_error_code() {
	return std::error_code(::GetLastError(), std::system_category());
}

}  // namespace base::win