#pragma once

namespace base {

class NonCopyable {
public:
	NonCopyable() = default;

	NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&&) = default;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

class NonCopyMovable : public NonCopyable {
public:
	NonCopyMovable() = default;

	NonCopyMovable(NonCopyMovable&&) = delete;
	NonCopyMovable& operator=(NonCopyMovable&&) = delete;
};

}  // namespace base