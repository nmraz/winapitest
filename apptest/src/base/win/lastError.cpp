#include "lastError.h"

#include <Windows.h>

namespace base {
namespace win {

[[noreturn]] void throwLastError(const char* what) {
	throw std::system_error(::GetLastError(), std::system_category(), what);
}

std::error_code lastErrorCode() {
	return std::error_code(::GetLastError(), std::system_category());
}

}  // namespace win
}  // namespace base