#pragma once

#include "base/NonCopyable.h"

namespace base {
namespace win {

using handle = void*;

class scoped_handle : public non_copyable {
public:
	scoped_handle();

	explicit scoped_handle(handle handle);
	scoped_handle(scoped_handle&& rhs) noexcept;

	~scoped_handle();

	scoped_handle& operator=(handle handle);
	scoped_handle& operator=(scoped_handle rhs);

	handle get() const { return handle_; }
	void swap(scoped_handle& other);
	void release();

	explicit operator bool() const;

private:
	handle handle_;
};

}  // namespace win
}  // namespace base