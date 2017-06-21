#pragma once

#include <string>
#include <string_view>

namespace base {

std::wstring u8_to_u16(std::string_view str);

std::string u16_to_u8(std::wstring_view str);

}  // namespace base