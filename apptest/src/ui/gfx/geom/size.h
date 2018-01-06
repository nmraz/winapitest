#pragma once

#include <algorithm>
#include <type_traits>

namespace gfx {

template<typename T>
struct size {
  constexpr size();
  constexpr size(T width, T height);

  template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr size(const size<U>& other);

  constexpr T width() const { return width_; }
  constexpr T height() const { return height_; }

  constexpr T area() const { return width * height; }
  constexpr bool empty() const;

  constexpr void set(T new_width, T new_height);
  constexpr void set_width(T new_width);
  constexpr void set_height(T new_height);

  constexpr void expand_to(const size& other);
  constexpr void shrink_to(const size& other);

private:
  T width_, height_;
};


template<typename T>
constexpr size<T>::size()
  : size(T(0), T(0)) {
}

template<typename T>
constexpr size<T>::size(T width, T height) {
  set(width, height);
}


template<typename T>
template<typename U, typename>
constexpr size<T>::size(const size<U>& other)
  : width_(other.width_)
  , height_(other.height_) {
}


template<typename T>
constexpr bool size<T>::empty() const {
  return width_ == 0 || height_ == 0;
}


template<typename T>
constexpr void size<T>::set(T new_width, T new_height) {
  set_width(new_width);
  set_height(new_height);
}

template<typename T>
constexpr void size<T>::set_width(T new_width) {
  width_ = std::max(new_width, T(0));
}

template<typename T>
constexpr void size<T>::set_height(T new_height) {
  height_ = std::max(new_height, T(0));
}


template<typename T>
constexpr void size<T>::expand_to(const size& other) {
  width_ = std::max(width_, other.width_);
  height_ = std::max(height_, other.height_);
}

template<typename T>
constexpr void size<T>::shrink_to(const size& other) {
  width_ = std::min(width_, other.width_);
  height_ = std::min(height_, other.height_);
}


template<typename T>
constexpr size<T>& operator+=(size<T>& lhs, const size<T>& rhs) {
  lhs.set(lhs.width() + rhs.width(), lhs.height() + rhs.height());
  return rhs;
}

template<typename T>
constexpr size<T> operator+(const size<T>& lhs, const size<T>& rhs) {
  return { lhs.width() + rhs.width(), lhs.height() + rhs.height() };
}


template<typename T>
constexpr size<T>& operator-=(size<T>& lhs, const size<T>& rhs) {
  lhs.set(lhs.width() - rhs.width(), lhs.height() - rhs.height());
  return rhs;
}

template<typename T>
constexpr size<T> operator-(const size<T>& lhs, const size<T>& rhs) {
  return { lhs.width() - rhs.width(), lhs.height() - rhs.height() };
}


template<typename T, typename U>
constexpr size<T>& operator*=(size<T>& lhs, U value) {
  lhs.set(static_cast<T>(lhs.width() * value), static_cast<T>(lhs.height() * value));
  return lhs;
}

template<typename T, typename U>
constexpr size<T> operator*(const size<T>& lhs, U value) {
  return { static_cast<T>(lhs.width() * value), static_cast<T>(lhs.height() * value) };
}

template<typename T, typename U>
constexpr size<T> operator*(U value, const size<T>& rhs) {
  return rhs * value;
}


template<typename T, typename U>
constexpr size<T>& operator/=(size<T>& lhs, U value) {
  lhs.set(static_cast<T>(lhs.width() / value), static_cast<T>(lhs.height() / value));
  return lhs;
}

template<typename T, typename U>
constexpr size<T> operator/(const size<T>& lhs, U value) {
  return { static_cast<T>(lhs.width() / value), static_cast<T>(lhs.height() / value) };
}


template<typename T>
constexpr bool operator==(const size<T>& lhs, const size<T>& rhs) {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

template<typename T>
constexpr bool operator!=(const size<T>& lhs, const size<T>& rhs) {
  return !(lhs == rhs);
}


template<typename T>
constexpr size<T> lerp(const size<T>& from, const size<T>& to, double t) {
  return {
    static_cast<T>(from.width() + (to.width() - from.width()) * t),
    static_cast<T>(from.height() + (to.height() - from.height()) * t)
  };
}


using sizei = size<int>;
using sizef = size<float>;

}  // namepsace gfx