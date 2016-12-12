#include "unicode.h"

#include <codecvt>
#include <locale>

using Conv = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>>;

namespace base {

std::wstring u8ToU16(std::string_view str) {
	return Conv().from_bytes(str.data());
}

std::string u16ToU8(std::wstring_view str) {
	return Conv().to_bytes(str.data());
}

}  // namespace base