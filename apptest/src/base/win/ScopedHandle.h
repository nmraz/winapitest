#pragma once

#include "base/NonCopyable.h"

namespace base {
namespace win {

using Handle = void*;

class ScopedHandle : public NonCopyable {
public:
	ScopedHandle();

	explicit ScopedHandle(Handle handle);
	ScopedHandle(ScopedHandle&& rhs) noexcept;

	~ScopedHandle();

	ScopedHandle& operator=(Handle handle);
	ScopedHandle& operator=(ScopedHandle rhs);

	Handle get() { return mHandle; }
	void swap(ScopedHandle& other);
	void release();

	explicit operator bool() const;

private:
	Handle mHandle;
};

}  // namespace win
}  // namespace base