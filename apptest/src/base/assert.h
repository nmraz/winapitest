#pragma once

#include <sstream>

#define ASSERT_IMPL(COND) (COND) ? (void) 0 : \
  ::base::impl::assert_voidify() | ::base::impl::failed_assertion(__FILE__, __LINE__, #COND, __FUNCTION__)

#ifndef NDEBUG
#define ASSERT(COND) ASSERT_IMPL(COND)
#else
// can't just make this empty, as we still need streaming support
#define ASSERT(COND) __assume(COND), ASSERT_IMPL(true)
#endif

namespace base::impl {

class failed_assertion {
public:
  failed_assertion(const char* file, int line, const char* cond, const char* func);
  ~failed_assertion();

  template<typename T>
  failed_assertion& operator<<(T&& val) {
    stream_ << std::forward<T>(val);
    return *this;
  }

private:
  std::ostringstream stream_;
  const char* file_;
  int line_;
};

struct assert_voidify {
  void operator|(const failed_assertion&) {}
};

}  // namespace base::impl