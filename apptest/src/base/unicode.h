#pragma once

#include <string>
#include <string_view>

namespace base {

std::wstring u8ToU16(std::string_view str);

std::string u16ToU8(std::wstring_view str);

}  // namespace base