#pragma once

#include <atomic>
#include <type_traits>
#include <Unknwn.h>

namespace base::win {
namespace impl {

template<typename... Bases, typename T>
HRESULT query_interface([[maybe_unused]] T* obj, [[maybe_unused]] REFIID iid, void** out);

template<typename First, typename... Rest, typename T>
inline HRESULT query_first_interface(T* obj, REFIID iid, void** out) {
  if (iid == __uuidof(First)) {
    *out = static_cast<First*>(obj);
    obj->AddRef();
    return S_OK;
  }
  return query_interface<Rest...>(obj, iid, out);
}

template<typename... Bases, typename T>
inline HRESULT query_interface([[maybe_unused]] T* obj, [[maybe_unused]] REFIID iid, void** out) {
  if constexpr (sizeof...(Bases) > 0) {
    return query_first_interface(obj, iid, out);
  } else {
    *out = nullptr;
    return E_NOINTERFACE;
  }
}

}  // namespace impl

template<typename... Bases, typename T>
HRESULT query_interface(T* obj, REFIID iid, void** out) {
  static_assert(std::is_base_of_v<IUnknown, T>, "Queryable object must derive from IUnknown");

  return impl::query_interface(obj, iid, out);
}


template<typename T>
class com_impl_base : public T {
public:
  static_assert(std::is_base_of_v<IUnknown, T>, "T must be a COM interface");

  STDMETHOD_(ULONG, AddRef)() override;
  STDMETHOD_(ULONG, Release)() override;

protected:
  virtual ~com_impl_base() {}

private:
  std::atomic<ULONG> ref_count_ = 1;
};


template<typename T>
STDMETHODIMP_(ULONG) com_impl_base<T>::AddRef() {
  return ref_count_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
STDMETHODIMP_(ULONG) com_impl_base<T>::Release() {
  ULONG new_count = ref_count_.fetch_sub(1, std::memory_order_acq_rel);
  if (!new_count) {
    delete this;
  }

  return new_count;
}


template<typename T, typename... Bases>
class com_impl : public com_impl_base<T> {
public:
  STDMETHOD(QueryInterface)(REFIID iid, void** out) override;
};


template<typename T, typename... Bases>
STDMETHODIMP com_impl<T, Bases...>::QueryInterface(REFIID iid, void** out) {
  return query_interface<IUnknown, Bases..., T>(this, iid, out);
}

}  // namespace base::win