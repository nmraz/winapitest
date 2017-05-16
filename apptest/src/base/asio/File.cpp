#include "File.h"
#include "base/win/lastError.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

struct OverlappedEx : OVERLAPPED {
	File::CompleteCallback callback;
};

auto makeOverlapped(std::int64_t offset, File::CompleteCallback& callback) {
	auto overlapped = std::make_unique<OverlappedEx>();

	LARGE_INTEGER offsetParts;
	offsetParts.QuadPart = offset;

	overlapped->callback = std::move(callback);
	overlapped->Offset = offsetParts.LowPart;
	overlapped->OffsetHigh = offsetParts.HighPart;

	return overlapped;
}


void IoCompleteCallback(DWORD err, DWORD bytesTransferred, OVERLAPPED* overlapped) noexcept {
	std::unique_ptr<OverlappedEx> overlappedEx(static_cast<OverlappedEx*>(overlapped));
	std::error_code ec(err, std::system_category());

	overlappedEx->callback(bytesTransferred, ec);
}

}  // namespace


File::File(std::string_view name, Mode mode) {
	open(name, mode);
}


void File::open(std::string_view name, Mode mode) {

}

void File::close() {
	mHandle.release();
}


void File::read(std::int64_t offset, void* buf, unsigned long count, CompleteCallback callback) {
	auto overlapped = makeOverlapped(offset, callback);

	if (!::ReadFileEx(mHandle.get(), buf, count, overlapped.get(), IoCompleteCallback)) {
		win::throwLastError("Failed to initiate file read");
	}
	overlapped.release();
}

void File::write(std::int64_t offset, const void* buf, unsigned long count, CompleteCallback callback) {
	auto overlapped = makeOverlapped(offset, callback);

	if (!::WriteFileEx(mHandle.get(), buf, count, overlapped.get(), IoCompleteCallback)) {
		win::throwLastError("Failed to initiate file write");
	}
	overlapped.release();
}


std::int64_t File::length() {
	LARGE_INTEGER length;
	::GetFileSizeEx(mHandle.get(), &length);
	return length.QuadPart;
}

}  // namespace base