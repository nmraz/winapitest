#include "assert.h"

#include "base/logging/logging.h"
#include <exception>

namespace base::impl {

failed_assertion::failed_assertion(const char* file, int line, const char* cond, const char* func)
  : file_(file)
  , line_(line) {
  stream_ << "[" << func << "] Assertion `" << cond << "` failed: ";
}

#pragma warning(push)
#pragma warning(disable: 4722)  // dtor never returns
failed_assertion::~failed_assertion() {
  LOG_FULL(error, file_, line_) << stream_.str();
  std::terminate();
}
#pragma warning(pop)

}  // namespace base::impl