#pragma once

#include <atomic>
#include <Unknwn.h>

namespace base::win {

template<typename... Bases, typename T>
HRESULT query_interface(T* obj, REFIID iid, void** out);

template<typename First, typename... Rest, typename T>
inline HRESULT do_query_interface(T* obj, REFIID iid, void** out) {
	if (iid == __uuidof(First)) {
		*out = static_cast<First>(obj);
		return S_OK;
	}
	return query_interface<Rest...>(obj, iid, out);
}

template<typename... Bases, typename T>
inline HRESULT query_interface(T* obj, REFIID iid, void** out) {
	if constexpr (sizeof...(Bases) > 0) {
		return do_query_interface(obj, iid, out);
	} else {
		*out = nullptr;
		return E_NOTIMPL;
	}
}


template<typename T>
class com_impl : public T {
public:
	STDMETHOD_(ULONG, AddRef)() override;
	STDMETHOD_(ULONG, Release)() override;

	STDMETHOD(QueryInterface)(REFIID iid, void** out) override;

protected:
	virtual ~com_impl() {}

private:
	std::atomic<ULONG> ref_count_;
};


template<typename T>
ULONG STDMETHODCALLTYPE com_impl<T>::AddRef() {
	return ref_count_.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
ULONG STDMETHODCALLTYPE com_impl<T>::Release() {
	ULONG new_count = ref_count_.fetch_sub(1, std::memory_order_acq_rel);
	if (!new_count) {
		delete this;
	}

	return new_count;
}


template<typename T>
STDMETHODIMP com_impl<T>::QueryInterface(REFIID iid, void** out) {
	return query_interface<IUnknown, T>(this, iid, out);
}

}  // namespace base::win