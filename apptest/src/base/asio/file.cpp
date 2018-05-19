#include "file.h"

#include "base/assert.h"
#include "base/unicode.h"
#include "base/win/last_error.h"
#include <memory>
#include <utility>
#include <Windows.h>

namespace base {
namespace {

struct overlappedex : OVERLAPPED {
  promise<unsigned long> prom;
};

auto make_overlapped(file::offset_type offset) {
  auto overlapped = std::make_unique<overlappedex>();

  LARGE_INTEGER offsetParts;
  offsetParts.QuadPart = offset;

  overlapped->Offset = offsetParts.LowPart;
  overlapped->OffsetHigh = offsetParts.HighPart;

  return overlapped;
}


DWORD get_win_create_disp(file::create_disp disp) {
  switch (disp) {
  case file::create_disp::open_only:
    return OPEN_EXISTING;
  case file::create_disp::create_only:
    return CREATE_NEW;
  case file::create_disp::open_always:
    return OPEN_ALWAYS;
  case file::create_disp::create_always:
    return CREATE_ALWAYS;
  default:
    NOTREACHED() << "Invalid file creation disposition";
    return 0;
  }
}

DWORD get_win_access(int desired_access) {
  DWORD ret = 0;

  if (desired_access & file::in) {
    ret |= GENERIC_READ;
  }
  if (desired_access & file::out) {
    ret |= GENERIC_WRITE;
  }
  return ret;
}

DWORD get_win_share_mode(int mode) {
  DWORD ret = 0;

  if (mode & file::share_read) {
    ret |= FILE_SHARE_READ;
  }
  if (mode & file::share_write) {
    ret |= FILE_SHARE_WRITE;
  }
  if (mode & file::share_del) {
    ret |= FILE_SHARE_DELETE;
  }
  return ret;
}


void CALLBACK on_io_complete(DWORD err, DWORD bytes_transferred, OVERLAPPED* win_overlapped) noexcept {
  std::unique_ptr<overlappedex> overlapped(static_cast<overlappedex*>(win_overlapped));

  if (err) {
    overlapped->prom.set_exception(std::system_error(err, std::system_category()));
  } else {
    overlapped->prom.set(bytes_transferred);
  }
}

}  // namespace


file::file(std::string_view name, int desired_access, create_disp disp, int share) {
  open(name, desired_access, disp, share);
}


void file::open(std::string_view name, int desired_access, create_disp disp, int share) {
  handle_.set(::CreateFileW(
    widen(name).c_str(),
    get_win_access(desired_access),
    get_win_share_mode(share),
    nullptr,
    get_win_create_disp(disp),
    FILE_FLAG_OVERLAPPED,
    nullptr
  ));

  if (!handle_) {
    win::throw_last_error("Failed to open file");
  }
}

void file::close() {
  handle_.release();
}


file::complete_type file::read(offset_type offset, void* buf, unsigned long count) {
  auto overlapped = make_overlapped(offset);
  auto fut = overlapped->prom.get_future();

  if (!::ReadFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
    overlapped->prom.set_exception(std::system_error(win::last_error()));
  } else {
    overlapped.release();
  }

  return fut;
}

file::complete_type file::write(offset_type offset, const void* buf, unsigned long count) {
  auto overlapped = make_overlapped(offset);
  auto fut = overlapped->prom.get_future();

  if (!::WriteFileEx(handle_.get(), buf, count, overlapped.get(), on_io_complete)) {
    overlapped->prom.set_exception(std::system_error(win::last_error()));
  } else {
    overlapped.release();
  }

  return fut;
}


file::offset_type file::length() {
  LARGE_INTEGER length;
  ::GetFileSizeEx(handle_.get(), &length);
  return length.QuadPart;
}

}  // namespace base