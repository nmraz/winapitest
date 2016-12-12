#include "loggingSinks.h"

#include "base/unicode.h"
#include <iostream>
#include <Windows.h>

namespace logging {

FileSink::FileSink(std::string_view name)
	: mFile(base::u8ToU16(name)) {
	mFile << "\xef\xbb\xbf";  // write utf-8 bom for windows compatibility
}

void FileSink::write(const char* str) {
	mFile << str;
}


void StdoutSink::write(const char* str) {
	std::cout << str << std::flush;
}


void DebuggerSink::write(const char* str) {
	::OutputDebugStringW(base::u8ToU16(str).c_str());
}

}  // namespace logging