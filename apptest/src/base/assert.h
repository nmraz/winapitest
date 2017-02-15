#pragma once

#include <sstream>

#define ASSERT_IMPL(COND) (COND) ? (void) 0 : \
	::base::impl::AssertVoidify() | ::base::impl::FailedAssertion(__FILE__, __LINE__, #COND, __FUNCTION__)

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
	FailedAssertion(const char* file, int line, const char* cond, const char* func);
	~FailedAssertion();

	template<typename T>
	FailedAssertion& operator<<(T&& val) {
		mStream << std::forward<T>(val);
		return *this;
	}

private:
	std::ostringstream mStream;
	const char* mFile;
	int mLine;
};

struct AssertVoidify {
	void operator|(const FailedAssertion&) {}
};

}  // namespace impl
}  // namespace base