#pragma once

#include <stdexcept>
#include <system_error>
#include <utility>
#include <winerror.h>

namespace base::win {

template<typename F>
HRESULT exception_boundary(F&& f) noexcept {
	try {
		std::forward<F>(f)();
		return S_OK;
	} catch (const std::bad_alloc&) {
		return E_OUTOFMEMORY;
	} catch (const std::system_error& e) {
		auto code = e.code();
		if (code.category() == std::system_category()) {
			return HRESULT_FROM_WIN32(code.value());
		}
		return E_FAIL;
	} catch (...) {
		return E_FAIL;
	}
}

}  // namespace base::win