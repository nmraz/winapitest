#include "assert.h"

#include <intrin.h>

namespace base {
namespace impl {

FailedAssertion::~FailedAssertion() {
	__debugbreak();
}

}  // namespace impl
}  // namespace base