#include "unicode.h"

#include <Windows.h>
#include <stdexcept>

namespace base {

std::wstring u8_to_u16(std::string_view str) {
	if (str.empty()) {
		return L"";
	}
	int in_size = static_cast<int>(str.size());
	int out_size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), in_size, nullptr, 0);
	if (!out_size) {
		throw std::range_error("Bad unicode conversion");
	}
	std::wstring out(out_size, '\0');
	::MultiByteToWideChar(CP_UTF8, 0, str.data(), in_size, out.data(), out_size);
	return out;
}

std::string u16_to_u8(std::wstring_view str) {
	if (str.empty()) {
		return "";
	}
	int in_size = static_cast<int>(str.size());
	int out_size = ::WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), in_size, nullptr, 0, nullptr, nullptr);
	if (!out_size) {
		throw std::range_error("Bad unicode conversion");
	}
	std::string out(out_size, '\0');
	::WideCharToMultiByte(CP_UTF8, 0, str.data(), in_size, out.data(), out_size, nullptr, nullptr);
	return out;
}

}  // namespace base