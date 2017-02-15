#include "assert.h"

#include "base/logging/logging.h"
#include <intrin.h>

namespace base {
namespace impl {

FailedAssertion::FailedAssertion(const char* file, int line, const char* cond, const char* func)
	: mFile(file)
	, mLine(line) {
	mStream << "[" << func << "] Assertion `" << cond << "` failed: ";
}

FailedAssertion::~FailedAssertion() {
	LOG_FULL(error, mFile, mLine) << mStream.str();
	__debugbreak();
}

}  // namespace impl
}  // namespace base