#pragma once

#include "base/win/ScopedHandle.h"
#include <cstdint>
#include <functional>
#include <iterator>
#include <string_view>
#include <system_error>

namespace base {

class file {
public:
	enum flags {
		open_only = 1 << 0,  // open only if exists
		create_only = 1 << 1,  // create only if doesn't exist

		open_always = 1 << 2,  // open or create
		create_always = 1 << 3,  // open and trucate or create
		
		in = 1 << 4,  // read
		out = 1 << 5  // write
	};

	using offset_type = std::int64_t;

	using complete_callback = std::function<void(unsigned long, const std::error_code&)>;

	file() = default;
	file(std::string_view name, int open_flags);

	void open(std::string_view name, int open_flags);
	void close();

	template<typename Buffer, typename Cb>
	void read(offset_type offset, Buffer&& buf, Cb&& callback);

	template<typename Buffer, typename Cb>
	void write(offset_type offset, Buffer&& buf, Cb&& callback);

	void read(offset_type offset, void* buf, unsigned long count, complete_callback callback);
	void write(offset_type offset, const void* buf, unsigned long count, complete_callback callback);

	offset_type length();

private:
	win::scoped_handle handle_;
};


template<typename Buffer, typename Cb>
void file::read(offset_type offset, Buffer&& buf, Cb&& callback) {
	read(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)), std::forward<Cb>(callback));
}

template<typename Buffer, typename Cb>
void file::write(offset_type offset, Buffer&& buf, Cb&& callback) {
	write(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)), std::forward<Cb>(callback));
}

}  // namespace base