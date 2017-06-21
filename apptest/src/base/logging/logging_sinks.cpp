#include "logging_sinks.h"

#include "base/unicode.h"
#include <iostream>
#include <Windows.h>

namespace logging {

file_sink::file_sink(std::string_view name)
	: file_(base::u8_to_u16(name)) {
	file_ << "\xef\xbb\xbf";  // write utf-8 bom for windows compatibility
}

void file_sink::write(const char* str) {
	file_ << str;
}


void stdout_sink::write(const char* str) {
	std::cout << str << std::flush;
}


void debugger_sink::write(const char* str) {
	::OutputDebugStringW(base::u8_to_u16(str).c_str());
}

}  // namespace logging