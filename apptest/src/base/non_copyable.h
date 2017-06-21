#pragma once

namespace base {

class non_copyable {
public:
	non_copyable() = default;

	non_copyable(non_copyable&&) = default;
	non_copyable& operator=(non_copyable&&) = default;

	non_copyable(const non_copyable&) = delete;
	non_copyable& operator=(const non_copyable&) = delete;
};

class non_copy_movable : public non_copyable {
public:
	non_copy_movable() = default;

	non_copy_movable(non_copy_movable&&) = delete;
	non_copy_movable& operator=(non_copy_movable&&) = delete;
};

}  // namespace base