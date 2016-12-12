#pragma once

#include "base/logging/logging.h"

#define ASSERT_IMPL(cond) (cond) ? (void) 0 : (::base::impl::FailedAssertion(), false) ? (void) 0 : \
	LOG(error) << "assertion `" #cond "` failed: "

#ifndef NDEBUG
#define ASSERT(cond) ASSERT_IMPL(cond)
#else
// can't just make this empty, as we still need streaming support
#define ASSERT(cond) ASSERT_IMPL(true)
#endif

namespace base {
namespace impl {

class FailedAssertion {
public:
	~FailedAssertion();
};

}  // namespace impl
}  // namespace base