#include "File.h"
#include "base/unicode.h"
#include "base/win/lastError.h"
#include <memory>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

struct OverlappedEx : OVERLAPPED {
	File::CompleteCallback callback;
};

auto makeOverlapped(File::Offset offset, File::CompleteCallback& callback) {
	auto overlapped = std::make_unique<OverlappedEx>();

	LARGE_INTEGER offsetParts;
	offsetParts.QuadPart = offset;

	overlapped->callback = std::move(callback);
	overlapped->Offset = offsetParts.LowPart;
	overlapped->OffsetHigh = offsetParts.HighPart;

	return overlapped;
}


void CALLBACK IoCompleteCallback(DWORD err, DWORD bytesTransferred, OVERLAPPED* overlapped) noexcept {
	std::unique_ptr<OverlappedEx> overlappedEx(static_cast<OverlappedEx*>(overlapped));
	std::error_code ec(err, std::system_category());

	overlappedEx->callback(bytesTransferred, ec);
}

}  // namespace


File::File(std::string_view name, int flags) {
	open(name, flags);
}


void File::open(std::string_view name, int flags) {
	DWORD access = 0, createDisp = 0;

	if (flags & Flags::openOnly) {
		createDisp = OPEN_EXISTING;
	} else if (flags & Flags::createOnly) {
		createDisp = CREATE_NEW;
	} else if (flags & Flags::openAlways) {
		createDisp = OPEN_ALWAYS;
	} else if (flags & Flags::createAlways) {
		createDisp = CREATE_ALWAYS;
	}

	if (flags & Flags::in) {
		access |= GENERIC_READ;
	}
	if (flags & Flags::out) {
		access |= GENERIC_WRITE;
	}

	mHandle = ::CreateFileW(u8ToU16(name).c_str(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, createDisp, FILE_FLAG_OVERLAPPED, nullptr);
	if (mHandle.get() == INVALID_HANDLE_VALUE) {
		win::throwLastError("Failed to open file");
	}
}

void File::close() {
	mHandle.release();
}


void File::read(Offset offset, void* buf, unsigned long count, CompleteCallback callback) {
	auto overlapped = makeOverlapped(offset, callback);

	if (!::ReadFileEx(mHandle.get(), buf, count, overlapped.get(), IoCompleteCallback)) {
		win::throwLastError("Failed to initiate file read");
	}
	overlapped.release();
}

void File::write(Offset offset, const void* buf, unsigned long count, CompleteCallback callback) {
	auto overlapped = makeOverlapped(offset, callback);

	if (!::WriteFileEx(mHandle.get(), buf, count, overlapped.get(), IoCompleteCallback)) {
		win::throwLastError("Failed to initiate file write");
	}
	overlapped.release();
}


File::Offset File::length() {
	LARGE_INTEGER length;
	::GetFileSizeEx(mHandle.get(), &length);
	return length.QuadPart;
}

}  // namespace base