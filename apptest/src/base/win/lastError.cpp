#include "lastError.h"

#include <system_error>
#include <Windows.h>

namespace base {
namespace win {

[[noreturn]] void throwLastError(const char* what) {
	throw std::system_error(::GetLastError(), std::system_category(), what);
}

}  // namespace win
}  // namespace base