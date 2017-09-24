#include "assert.h"

#include "base/logging/logging.h"
#include <intrin.h>

namespace base::impl {

failed_assertion::failed_assertion(const char* file, int line, const char* cond, const char* func)
  : file_(file)
  , line_(line) {
  stream_ << "[" << func << "] Assertion `" << cond << "` failed: ";
}

failed_assertion::~failed_assertion() {
  LOG_FULL(error, file_, line_) << stream_.str();
  __debugbreak();
}

}  // namespace base::impl