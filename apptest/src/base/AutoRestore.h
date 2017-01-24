#pragma once

#include "base/NonCopyable.h"
#include <utility>

namespace base {

template<typename T>
class AutoRestore : public NonCopyMovable {
public:
	AutoRestore(T& var)
		: mVar(&var)
		, mOldVal(var) {
	}

	template<typename U>
	AutoRestore(T& var, U&& newVal)
		: mVar(&var)
		, mOldVal(std::move(var)) {
		*mVar = std::forward<U>(newVal);
	}

	~AutoRestore() {
		*mVar = std::move(mOldVal);
	}

private:
	T* mVar;
	T mOldVal;
};

}