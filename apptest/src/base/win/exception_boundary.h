#pragma once

#include <stdexcept>
#include <system_error>
#include <utility>
#include <winerror.h>

namespace base::win {
namespace except_bound_impl {

template<typename T>
struct dispatch_tag {};

template<typename F>
using disptach_tag_for = dispatch_tag<decltype(std::declval<F>()())>;

template<typename F>
inline HRESULT call_f(F&& f, dispatch_tag<HRESULT>) {
  return std::forward<F>(f)();
}

template<typename F, typename Ret>
inline HRESULT call_f(F&& f, dispatch_tag<Ret>) {
  std::forward<F>(f)();
  return S_OK;
}

}  // namespace except_bound_impl

template<typename F>
HRESULT exception_boundary(F&& f) noexcept {
  try {
    return except_bound_impl::call_f(std::forward<F>(f), except_bound_impl::disptach_tag_for<F>{});
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