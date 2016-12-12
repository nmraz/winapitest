#pragma once

namespace base {
namespace win {

[[noreturn]] void throwLastError(const char* what = "");

}  // namespace win
}  // namespace base
