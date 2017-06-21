#pragma once

#include "base/assert.h"
#include "base/NonCopyable.h"
#include <memory>
#include <type_traits>
#include <utility>

namespace base {

template<typename T>
class weak_factory;

template<typename T>
class weak_ptr {
public:
	constexpr weak_ptr()
		: ref_(nullptr) {}
	constexpr weak_ptr(std::nullptr_t)
		: weak_ptr() {}

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	weak_ptr(const weak_ptr<U>& rhs);

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	weak_ptr(weak_ptr<U>&& rhs);

	weak_ptr& operator=(weak_ptr rhs);

	T* get() const { return ref_ ? *ref_ : nullptr; }
	T* operator->() const;
	T& operator*() const { return *operator->(); }

	void reset() { ref_ = nullptr; }
	void swap(weak_ptr& rhs) { std::swap(ref_, rhs.ref_); }

	explicit operator bool() const { return get() != nullptr; }

private:
	friend weak_factory<T>;
	template<typename U> friend class weak_ptr;

	explicit weak_ptr(std::shared_ptr<T* const> ref)
		: ref_(std::move(ref)) {}

	std::shared_ptr<T* const> ref_;
};

template<typename T>
template<typename U, typename>
weak_ptr<T>::weak_ptr(const weak_ptr<U>& rhs)
	: ref_(rhs.ref_) {}

template<typename T>
template<typename U, typename>
weak_ptr<T>::weak_ptr(weak_ptr<U>&& rhs)
	: ref_(std::move(rhs.ref_)) {}


template<typename T>
weak_ptr<T>& weak_ptr<T>::operator=(weak_ptr rhs) {
	rhs.swap(*this);
	return *this;
}


template<typename T>
T* weak_ptr<T>::operator->() const {
	T* ptr = get();
	ASSERT(ptr) << "Null pointer dereference";
	return ptr;
}


// Relational operators

template<typename T, typename U>
inline bool operator==(const weak_ptr<T>& rhs, const weak_ptr<U>& lhs) {
	return rhs.get() == lhs.get();
}

template<typename T, typename U>
inline bool operator!=(const weak_ptr<T>& rhs, const weak_ptr<U>& lhs) {
	return !(rhs == lhs);
}

template<typename T>
inline bool operator==(const weak_ptr<T>& ptr, std::nullptr_t) {
	return ptr.get() == nullptr;
}

template<typename T>
inline bool operator==(std::nullptr_t, const weak_ptr<T>& ptr) {
	return ptr == nullptr;
}

template<typename T>
inline bool operator!=(const weak_ptr<T>& ptr, std::nullptr_t) {
	return !(ptr == nullptr);
}

template<typename T>
inline bool operator!=(std::nullptr_t, const weak_ptr<T>& ptr) {
	return ptr != nullptr;
}


template<typename T>
class weak_factory : non_copy_movable {
public:
	explicit weak_factory(T* ptr)
		: ref_(std::make_shared<T*>(ptr)) {}

	weak_ptr<T> getWeakPtr() const { return weak_ptr<T>(ref_); }

	void invalidate() { *ref_ = nullptr; }
	~weak_factory() { invalidate(); }

private:
	std::shared_ptr<T*> ref_;
};

}  // namespace base