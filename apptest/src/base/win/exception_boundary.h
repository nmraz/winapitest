#pragma once

#include <stdexcept>
#include <system_error>
#include <type_traits>
#include <utility>
#include <winerror.h>

namespace base::win {

template<typename F>
HRESULT exception_boundary(F&& f) noexcept {
  try {
    if constexpr (std::is_same_v<std::decay_t<decltype(std::forward<F>(f)())>, HRESULT>) {
      return std::forward<F>(f)();
    } else {
      std::forward<F>(f)();
      return S_OK;
    }
  } catch (const std::bad_alloc&) {
    return E_OUTOFMEMORY;
  } catch (const std::out_of_range&) {
    return E_BOUNDS;
  } catch (const std::invalid_argument&) {
    return E_INVALIDARG;
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