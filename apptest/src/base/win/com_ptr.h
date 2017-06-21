#pragma once

#include "base/assert.h"
#include <type_traits>
#include <Unknwn.h>
#include <utility>

namespace base {
namespace win {

template<typename T>
class com_ptr {
public:
	constexpr com_ptr()
		: ptr_(nullptr) {}
	constexpr com_ptr(std::nullptr_t)
		: com_ptr() {}

	explicit com_ptr(T* ptr);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	com_ptr(const com_ptr<U>& rhs);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	com_ptr(com_ptr<U>&& rhs) noexcept;

	~com_ptr();

	com_ptr& operator=(T* ptr);
	com_ptr& operator=(com_ptr rhs);

	T* get() const { return ptr_; }
	void swap(com_ptr& other);
	void release();

	T* operator->() const;

	T** addr();
	void** void_addr() { return static_cast<void**>(addr()); }
	IUnknown** unknown_addr() { return static_cast<IUnknown**>(addr()); }

	template<typename U>
	HRESULT query_interface(com_ptr<U>& query);

	explicit operator bool() const { return !!ptr_; }

private:
	template<typename U>
	friend class com_ptr;

	T* ptr_;
};


template<typename T>
com_ptr<T>::com_ptr(T* ptr)
	: ptr_(ptr) {
}

template<typename T>
template<typename U, typename>
com_ptr<T>::com_ptr(com_ptr<U>&& rhs) noexcept
	: ptr_(rhs.ptr_) {
	rhs.ptr_ = nullptr;
}

template<typename T>
template<typename U, typename>
com_ptr<T>::com_ptr(const com_ptr<U>& rhs)
	: ptr_(rhs.ptr_) {
	if (ptr_) {
		ptr_->AddRef();
	}
}

template<typename T>
template<typename U, typename>
com_ptr<T>::com_ptr(com_ptr<U>&& rhs) noexcept
	: ptr_(rhs.ptr_) {
	rhs.ptr_ = nullptr;
}


template<typename T>
com_ptr<T>::~com_ptr() {
	release();
}


template<typename T>
com_ptr<T>& com_ptr<T>::operator=(T* ptr) {
	release();
	ptr_ = ptr;
}

template<typename T>
com_ptr<T>& com_ptr<T>::operator=(com_ptr rhs) {
	rhs.swap(*this);
	return *this;
}


template<typename T>
void com_ptr<T>::swap(com_ptr& other) {
	std::swap(ptr_, other.ptr_);
}


template<typename T>
void com_ptr<T>::release() {
	if (ptr_) {
		ptr_->Release();
		ptr_ = nullptr;
	}
}


template<typename T>
T* com_ptr<T>::operator->() const {
	ASSERT(ptr_) << "Null pointer dereference";
	return ptr_;
}


template<typename T>
T** com_ptr<T>::addr() {
	ASSERT(!ptr_) << "Pointer must be empty to recieve value";
	return &ptr_;
}


template<typename T>
template<typename U>
HRESULT com_ptr<T>::query_interface(com_ptr<U>& query) {
	ASSERT(ptr_) << "Null pointer dereference";
	return ptr_->QueryInterface(query.addr());
}


// Relational operators

template<typename T, typename U>
inline bool operator==(const com_ptr<T>& rhs, const com_ptr<U>& lhs) {
	return rhs.get() == lhs.get();
}

template<typename T, typename U>
inline bool operator!=(const com_ptr<T>& rhs, const com_ptr<U>& lhs) {
	return !(rhs == lhs);
}

template<typename T>
inline bool operator==(const com_ptr<T>& ptr, std::nullptr_t) {
	return ptr.get() == nullptr;
}

template<typename T>
inline bool operator==(std::nullptr_t, const com_ptr<T>& ptr) {
	return ptr == nullptr;
}

template<typename T>
inline bool operator!=(const com_ptr<T>& ptr, std::nullptr_t) {
	return !(ptr == nullptr);
}

template<typename T>
inline bool operator!=(std::nullptr_t, const com_ptr<T>& ptr) {
	return ptr != nullptr;
}

}  // namespace win
}  // namespace base