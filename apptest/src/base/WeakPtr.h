#pragma once

#include "base/assert.h"
#include "base/NonCopyable.h"
#include <memory>
#include <type_traits>
#include <utility>

namespace base {

template<typename T>
class WeakFactory;

template<typename T>
class WeakPtr {
public:
	constexpr WeakPtr()
		: mRef(nullptr) {}
	constexpr WeakPtr(std::nullptr_t)
		: WeakPtr() {}

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	WeakPtr(const WeakPtr<U>& rhs);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	WeakPtr(WeakPtr<U>&& rhs);

	WeakPtr& operator=(WeakPtr rhs);

	T* get() const { return mRef ? *mRef : nullptr; }
	T* operator->() const;
	T& operator*() const { return *operator->(); }

	void reset() { mRef = nullptr; }
	void swap(WeakPtr& rhs) { std::swap(mRef, rhs.mRef); }

	explicit operator bool() const { return get() != nullptr; }

private:
	friend WeakFactory<T>;
	template<typename U> friend class WeakPtr;

	explicit WeakPtr(std::shared_ptr<T* const> ref)
		: mRef(std::move(ref)) {}

	std::shared_ptr<T* const> mRef;
};

template<typename T>
template<typename U, typename>
WeakPtr<T>::WeakPtr(const WeakPtr<U>& rhs)
	: mRef(rhs.mRef) {}

template<typename T>
template<typename U, typename>
WeakPtr<T>::WeakPtr(WeakPtr<U>&& rhs)
	: mRef(std::move(rhs.mRef)) {}


template<typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr rhs) {
	rhs.swap(*this);
	return *this;
}


template<typename T>
T* WeakPtr<T>::operator->() const {
	T* ptr = get();
	ASSERT(ptr) << "Null pointer dereference";
	return ptr;
}


// Relational operators

template<typename T, typename U>
inline bool operator==(const WeakPtr<T>& rhs, const WeakPtr<U>& lhs) {
	return rhs.get() == lhs.get();
}

template<typename T, typename U>
inline bool operator!=(const WeakPtr<T>& rhs, const WeakPtr<U>& lhs) {
	return !(rhs == lhs);
}

template<typename T>
inline bool operator==(const WeakPtr<T>& ptr, std::nullptr_t) {
	return ptr.get() == nullptr;
}

template<typename T>
inline bool operator==(std::nullptr_t, const WeakPtr<T>& ptr) {
	return ptr == nullptr;
}

template<typename T>
inline bool operator!=(const WeakPtr<T>& ptr, std::nullptr_t) {
	return !(ptr == nullptr);
}

template<typename T>
inline bool operator!=(std::nullptr_t, const WeakPtr<T>& ptr) {
	return ptr != nullptr;
}


template<typename T>
class WeakFactory : NonCopyMovable {
public:
	explicit WeakFactory(T* ptr)
		: mRef(std::make_shared<T*>(ptr)) {}

	WeakPtr<T> getWeakPtr() const { return WeakPtr<T>(mRef); }

	void invalidate() { *mRef = nullptr; }
	~WeakFactory() { invalidate(); }

private:
	std::shared_ptr<T*> mRef;
};

}  // namespace base