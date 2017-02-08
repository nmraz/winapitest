#pragma once

#include "base/assert.h"
#include <type_traits>
#include <Unknwn.h>
#include <utility>

namespace base {
namespace win {

template<typename T>
class ComPtr {
public:
	constexpr ComPtr()
		: mPtr(nullptr) {}
	constexpr ComPtr(std::nullptr_t)
		: ComPtr() {}

	explicit ComPtr(T* ptr);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	ComPtr(const ComPtr<U>& rhs);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	ComPtr(ComPtr<U>&& rhs) noexcept;

	~ComPtr();

	ComPtr& operator=(T* ptr);
	ComPtr& operator=(ComPtr rhs);

	T* get() const { return mPtr; }
	void swap(ComPtr& other);
	void release();

	T* operator->() const;

	T** addr();
	void** voidAddr() { return static_cast<void**>(addr()); }
	IUnknown** unknownAddr() { return reinterpret_cast<IUnknown**>(addr()); }

	template<typename U>
	HRESULT queryInterface(ComPtr<U>& query);

	explicit operator bool() const { return !!mPtr; }

private:
	template<typename U>
	friend class ComPtr;

	T* mPtr;
};


template<typename T>
ComPtr<T>::ComPtr(T* ptr)
	: mPtr(ptr) {
}

template<typename T>
template<typename U, typename>
ComPtr<T>::ComPtr(const ComPtr<U>& rhs)
	: mPtr(rhs.mPtr) {
	if (mPtr) {
		mPtr->AddRef();
	}
}

template<typename T>
template<typename U, typename>
ComPtr<T>::ComPtr(ComPtr<U>&& rhs) noexcept
	: mPtr(rhs.mPtr) {
	rhs.mPtr = nullptr;
}


template<typename T>
ComPtr<T>::~ComPtr() {
	release();
}


template<typename T>
ComPtr<T>& ComPtr<T>::operator=(T* ptr) {
	release();
	mPtr = ptr;
}

template<typename T>
ComPtr<T>& ComPtr<T>::operator=(ComPtr rhs) {
	rhs.swap(*this);
	return *this;
}


template<typename T>
void ComPtr<T>::swap(ComPtr& other) {
	std::swap(mPtr, other.mPtr);
}


template<typename T>
void ComPtr<T>::release() {
	if (mPtr) {
		mPtr->Release();
		mPtr = nullptr;
	}
}


template<typename T>
T* ComPtr<T>::operator->() const {
	ASSERT(mPtr) << "Null pointer dereference";
	return mPtr;
}


template<typename T>
T** ComPtr<T>::addr() {
	ASSERT(!mPtr) << "Pointer must be empty to recieve value";
	return &mPtr;
}


template<typename T>
template<typename U>
HRESULT ComPtr<T>::queryInterface(ComPtr<U>& query) {
	ASSERT(mPtr) << "Null pointer dereference";
	return mPtr->QueryInterface(query.addr());
}


// Relational operators

template<typename T, typename U>
inline bool operator==(const ComPtr<T>& rhs, const ComPtr<U>& lhs) {
	return rhs.get() == lhs.get();
}

template<typename T, typename U>
inline bool operator!=(const ComPtr<T>& rhs, const ComPtr<U>& lhs) {
	return !(rhs == lhs);
}

template<typename T>
inline bool operator==(const ComPtr<T>& ptr, std::nullptr_t) {
	return ptr.get() == nullptr;
}

template<typename T>
inline bool operator==(std::nullptr_t, const ComPtr<T>& ptr) {
	return ptr == nullptr;
}

template<typename T>
inline bool operator!=(const ComPtr<T>& ptr, std::nullptr_t) {
	return !(ptr == nullptr);
}

template<typename T>
inline bool operator!=(std::nullptr_t, const ComPtr<T>& ptr) {
	return ptr != nullptr;
}

}  // namespace win
}  // namespace base