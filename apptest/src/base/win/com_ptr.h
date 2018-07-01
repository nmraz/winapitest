#pragma once

#include "base/assert.h"
#include "base/win/last_error.h"
#include <type_traits>
#include <Unknwn.h>
#include <utility>

namespace base::win {

template<typename T>
class com_ptr {
public:
  static_assert(std::is_base_of_v<IUnknown, T>, "T must be a COM interface");

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
  com_ptr& operator=(com_ptr rhs) noexcept;

  T* get() const { return ptr_; }
  T* detatch() { return std::exchange(ptr_, nullptr); }
  void swap(com_ptr& other) noexcept;
  void release();

  T* operator->() const;

  T** addr();
  void** void_addr() { return reinterpret_cast<void**>(addr()); }
  IUnknown** unknown_addr() { return reinterpret_cast<IUnknown**>(addr()); }

  template<typename U>
  HRESULT query_interface(com_ptr<U>& query) const;
  template<typename U>
  com_ptr<U> as() const;
  template<typename U>
  com_ptr<U> maybe_as() const;

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
  return *this;
}

template<typename T>
com_ptr<T>& com_ptr<T>::operator=(com_ptr rhs) noexcept {
  rhs.swap(*this);
  return *this;
}


template<typename T>
void com_ptr<T>::swap(com_ptr& other) noexcept {
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
HRESULT com_ptr<T>::query_interface(com_ptr<U>& query) const {
  return (*this)->QueryInterface(query.addr());
}

template<typename T>
template<typename U>
com_ptr<U> com_ptr<T>::as() const {
  com_ptr<U> ret;
  throw_if_failed(query_interface(ret));
  return ret;
}

template<typename T>
template<typename U>
com_ptr<U> com_ptr<T>::maybe_as() const {
  com_ptr<U> ret;
  if (FAILED(query_interface(ret))) {
    return nullptr;
  }
  return ret;
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


template<typename T>
inline void swap(com_ptr<T>& lhs, com_ptr<T>& rhs) noexcept {
  lhs.swap(rhs);
}

}  // namespace base::win