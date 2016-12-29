#pragma once

#include "base/logging/logging.h"

#define ASSERT_IMPL(COND) (COND) ? (void) 0 : (::base::impl::FailedAssertion(), false) ? (void) 0 : \
	LOG(error) << "Assertion failed in " << __func__ << "(): "

#ifndef NDEBUG
#define ASSERT(COND) ASSERT_IMPL(COND)
#else
// can't just make this empty, as we still need streaming support
#define ASSERT(COND) ASSERT_IMPL(true)
#endif

namespace base {
namespace impl {

class FailedAssertion {
public:
	~FailedAssertion();
};

}  // namespace impl
}  // namespace base