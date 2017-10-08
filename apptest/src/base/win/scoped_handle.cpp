#include "scoped_handle.h"

#include <Windows.h>
#include <utility>

namespace base::win {

using handle = HANDLE;  // trigger compiler error if handle is the wrong type in the header

scoped_handle::scoped_handle()
  : handle_(nullptr) {}

scoped_handle::scoped_handle(handle handle)
  : handle_(handle) {}

scoped_handle::scoped_handle(scoped_handle&& rhs) noexcept
  : handle_(rhs.handle_) {
  rhs.handle_ = nullptr;
}


scoped_handle::~scoped_handle() {
  release();
}


scoped_handle& scoped_handle::operator=(handle handle) {
  scoped_handle(handle).swap(*this);

  return *this;
}

scoped_handle& scoped_handle::operator=(scoped_handle rhs) noexcept {
  rhs.swap(*this);

  return *this;
}


void scoped_handle::swap(scoped_handle& other) noexcept {
  std::swap(handle_, other.handle_);
}


void scoped_handle::release() {
  if (*this) {
    ::CloseHandle(handle_);
    handle_ = nullptr;
  }
}


scoped_handle::operator bool() const {
  return !!handle_;
}

}  // namespace base::win