#pragma once

#include "base/non_copyable.h"

namespace base::win {

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
  void swap(scoped_handle& other) noexcept;
  void release();

  explicit operator bool() const;

private:
  handle handle_;
};


inline void swap(scoped_handle& lhs, scoped_handle& rhs) {
  lhs.swap(rhs);
}

}  // namespace base::win