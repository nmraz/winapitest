#include "unicode.h"

#include <codecvt>
#include <locale>

using conv = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>>;

namespace base {

std::wstring u8_to_u16(std::string_view str) {
	return conv().from_bytes(str.data());
}

std::string u16_to_u8(std::wstring_view str) {
	return conv().to_bytes(str.data());
}

}  // namespace base