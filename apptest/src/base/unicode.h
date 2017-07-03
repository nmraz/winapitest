#pragma once

#include <string>
#include <string_view>

namespace base {

std::wstring widen(std::string_view str);

std::string narrow(std::wstring_view str);

}  // namespace base