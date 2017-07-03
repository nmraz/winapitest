#include "File.h"
#include "base/unicode.h"
#include "base/win/last_error.h"
#include <memory>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

struct overlappedex : OVERLAPPED {
	file::complete_callback callback;
};

auto make_overlapped(file::offset_type offset, file::complete_callback&& callback) {
	auto overlapped = std::make_unique<overlappedex>();

	LARGE_INTEGER offsetParts;
	offsetParts.QuadPart = offset;

	overlapped->callback = std::move(callback);
	overlapped->Offset = offsetParts.LowPart;
	overlapped->OffsetHigh = offsetParts.HighPart;

	return overlapped;
}


void CALLBACK on_io_complete(DWORD err, DWORD bytes_transferred, OVERLAPPED* overlapped) noexcept {
	std::unique_ptr<overlappedex> overlapped_ex(static_cast<overlappedex*>(overlapped));
	std::error_code ec(err, std::system_category());

	overlapped_ex->callback(bytes_transferred, ec);
}

}  // namespace


file::file(std::string_view name, int open_flags) {
	open(name, open_flags);
}


void file::open(std::string_view name, int open_flags) {
	DWORD access = 0, create_disp = 0;

	if (open_flags & flags::open_only) {
		create_disp = OPEN_EXISTING;
	} else if (open_flags & flags::create_only) {
		create_disp = CREATE_NEW;
	} else if (open_flags & flags::open_always) {
		create_disp = OPEN_ALWAYS;
	} else if (open_flags & flags::create_always) {
		create_disp = CREATE_ALWAYS;
	}

	if (open_flags & flags::in) {
		access |= GENERIC_READ;
	}
	if (open_flags & flags::out) {
		access |= GENERIC_WRITE;
	}

	handle_ = ::CreateFileW(widen(name).c_str(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, create_disp, FILE_FLAG_OVERLAPPED, nullptr);
	if (handle_.get() == INVALID_HANDLE_VALUE) {
		win::throw_last_error("Failed to open file");
	}
}

void file::close() {
	handle_.release();
}


void file::read(offset_type offset, void* buf, unsigned long count, complete_callback callback) {
	auto overlapped = make_overlapped(offset, std::move(callback));

	if (!::ReadFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
		overlapped->callback(0, win::last_error_code());
		return;
	}
	overlapped.release();
}

void file::write(offset_type offset, const void* buf, unsigned long count, complete_callback callback) {
	auto overlapped = make_overlapped(offset, std::move(callback));

	if (!::WriteFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
		overlapped->callback(0, win::last_error_code());
		return;
	}
	overlapped.release();
}


file::offset_type file::length() {
	LARGE_INTEGER length;
	::GetFileSizeEx(handle_.get(), &length);
	return length.QuadPart;
}

}  // namespace base