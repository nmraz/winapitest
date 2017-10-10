#include "file.h"
#include "base/unicode.h"
#include "base/win/last_error.h"
#include <memory>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

struct overlappedex : OVERLAPPED {
  file::complete_type::source_type source;
};

auto make_overlapped(file::offset_type offset) {
  auto overlapped = std::make_unique<overlappedex>();

  LARGE_INTEGER offsetParts;
  offsetParts.QuadPart = offset;

  overlapped->Offset = offsetParts.LowPart;
  overlapped->OffsetHigh = offsetParts.HighPart;

  return overlapped;
}


void CALLBACK on_io_complete(DWORD err, DWORD bytes_transferred, OVERLAPPED* win_overlapped) noexcept {
  std::unique_ptr<overlappedex> overlapped(static_cast<overlappedex*>(win_overlapped));

  if (err) {
    overlapped->source.set_exception(
      std::make_exception_ptr(std::system_error(err, std::system_category()))
    );
  } else {
    overlapped->source.set_value(bytes_transferred);
  }
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


file::complete_type file::read(offset_type offset, void* buf, unsigned long count) {
  auto overlapped = make_overlapped(offset);

  if (!::ReadFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
    overlapped->source.set_exception(
      std::make_exception_ptr(std::system_error(win::last_error_code()))
    );
    return overlapped->source.get_promise();
  } else {
    return overlapped.release()->source.get_promise();
  }
}

file::complete_type file::write(offset_type offset, const void* buf, unsigned long count) {
  auto overlapped = make_overlapped(offset);

  if (!::WriteFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
    overlapped->source.set_exception(
      std::make_exception_ptr(std::system_error(win::last_error_code()))
    );
    return overlapped->source.get_promise();
  } else {
    return overlapped.release()->source.get_promise();
  }
}


file::offset_type file::length() {
  LARGE_INTEGER length;
  ::GetFileSizeEx(handle_.get(), &length);
  return length.QuadPart;
}

}  // namespace base