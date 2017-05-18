#pragma once

#include "base/win/ScopedHandle.h"
#include <cstdint>
#include <functional>
#include <iterator>
#include <string_view>
#include <system_error>

namespace base {

class File {
public:
	enum Flags {
		openOnly = 1 << 0,  // open only if exists
		createOnly = 1 << 1,  // create only if doesn't exist

		openAlways = 1 << 2,  // open or create
		createAlways = 1 << 3,  // open and trucate or create
		
		in = 1 << 4,  // read
		out = 1 << 5  // write
	};

	using Offset = std::int64_t;

	using CompleteCallback = std::function<void(unsigned long, const std::error_code&)>;

	File() = default;
	File(std::string_view name, Flags flags);

	void open(std::string_view name, Flags flags);
	void close();

	template<typename Buffer, typename Cb>
	void read(Offset offset, Buffer&& buf, Cb&& callback);

	template<typename Buffer, typename Cb>
	void write(Offset offset, Buffer&& buf, Cb&& callback);

	void read(Offset offset, void* buf, unsigned long count, CompleteCallback callback);
	void write(Offset offset, const void* buf, unsigned long count, CompleteCallback callback);

	Offset length();

private:
	win::ScopedHandle mHandle;
};


template<typename Buffer, typename Cb>
void File::read(Offset offset, Buffer&& buf, Cb&& callback) {
	read(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)), std::forward<Cb>(callback));
}

template<typename Buffer, typename Cb>
void File::write(Offset offset, Buffer&& buf, Cb&& callback) {
	write(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)), std::forward<Cb>(callback));
}

}  // namespace base