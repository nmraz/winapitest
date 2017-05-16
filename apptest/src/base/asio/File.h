#pragma once

#include "base/win/ScopedHandle.h"
#include <cstdint>
#include <functional>
#include <string_view>
#include <system_error>

namespace base {

class File {
public:
	enum Mode {
		readMode,
		writeMode
	};

	using CompleteCallback = std::function<void(unsigned long, const std::error_code&)>;

	File() = default;
	File(std::string_view name, Mode mode);

	void open(std::string_view name, Mode mode);
	void close();

	void read(std::int64_t offset, void* buf, unsigned long count, CompleteCallback callback);
	void write(std::int64_t offset, const void* buf, unsigned long count, CompleteCallback callback);

	std::int64_t length();

private:
	win::ScopedHandle mHandle;
};

}  // namespace base