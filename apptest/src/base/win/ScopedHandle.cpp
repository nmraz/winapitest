#include "ScopedHandle.h"

#include <Windows.h>
#include <utility>

namespace base {
namespace win {

using Handle = HANDLE;  // trigger compiler error if Handle is the wrong type in the header

ScopedHandle::ScopedHandle()
	: mHandle(nullptr) {}

ScopedHandle::ScopedHandle(Handle handle)
	: mHandle(handle) {}

ScopedHandle::ScopedHandle(ScopedHandle&& rhs) noexcept
	: mHandle(rhs.mHandle) {
	rhs.mHandle = nullptr;
}


ScopedHandle::~ScopedHandle() {
	release();
}


ScopedHandle& ScopedHandle::operator=(Handle handle) {
	ScopedHandle(handle).swap(*this);

	return *this;
}

ScopedHandle& ScopedHandle::operator=(ScopedHandle rhs) {
	rhs.swap(*this);

	return *this;
}


void ScopedHandle::swap(ScopedHandle& other) {
	std::swap(mHandle, other.mHandle);
}


void ScopedHandle::release() {
	if (*this) {
		::CloseHandle(mHandle);
		mHandle = nullptr;
	}
}


ScopedHandle::operator bool() const {
	return !!mHandle;
}

}  // namespace win
}  // namespace base