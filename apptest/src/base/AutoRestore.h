#pragma once

#include "base/NonCopyable.h"
#include <utility>

namespace base {

template<typename T>
class auto_restore : public non_copy_movable {
public:
	auto_restore(T& var)
		: var_(&var)
		, old_val_(var) {
	}

	template<typename U>
	auto_restore(T& var, U&& newVal)
		: var_(&var)
		, old_val_(std::move(var)) {
		*var_ = std::forward<U>(newVal);
	}

	~auto_restore() {
		*var_ = std::move(old_val_);
	}

private:
	T* var_;
	T old_val_;
};

}