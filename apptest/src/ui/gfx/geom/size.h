#pragma once

#include <algorithm>
#include <type_traits>

namespace gfx {

template<typename Rep>
struct size {
  constexpr size();
  constexpr size(Rep width, Rep height);

  template<typename Rep2, typename = std::enable_if_t<std::is_convertible_v<Rep2, Rep>>>
  constexpr size(const size<Rep2>& other);

  constexpr Rep width() const { return width_; }
  constexpr Rep height() const { return height_; }

  constexpr Rep area() const { return width * height; }
  constexpr bool empty() const;

  constexpr void set(Rep new_width, Rep new_height);
  constexpr void set_width(Rep new_width);
  constexpr void set_height(Rep new_height);

  constexpr void expand_to(const size& other);
  constexpr void shrink_to(const size& other);

private:
  Rep width_, height_;
};


template<typename Rep>
constexpr size<Rep>::size()
  : size(Rep(0), Rep(0)) {
}

template<typename Rep>
constexpr size<Rep>::size(Rep width, Rep height) {
  set(width, height);
}


template<typename Rep>
template<typename Rep2, typename>
constexpr size<Rep>::size(const size<Rep2>& other)
  : width_(other.width_)
  , height_(other.height_) {
}


template<typename Rep>
constexpr bool size<Rep>::empty() const {
  return width_ == 0 || height_ == 0;
}


template<typename Rep>
constexpr void size<Rep>::set(Rep new_width, Rep new_height) {
  set_width(new_width);
  set_height(new_height);
}

template<typename Rep>
constexpr void size<Rep>::set_width(Rep new_width) {
  width_ = std::max(new_width, Rep(0));
}

template<typename Rep>
constexpr void size<Rep>::set_height(Rep new_height) {
  height_ = std::max(new_height, Rep(0));
}


template<typename Rep>
constexpr void size<Rep>::expand_to(const size& other) {
  width_ = std::max(width_, other.width_);
  height_ = std::max(height_, other.height_);
}

template<typename Rep>
constexpr void size<Rep>::shrink_to(const size& other) {
  width_ = std::min(width_, other.width_);
  height_ = std::min(height_, other.height_);
}


template<typename Rep>
constexpr size<Rep>& operator+=(size<Rep>& lhs, const size<Rep>& rhs) {
  lhs.set(lhs.width() + rhs.width(), lhs.height() + rhs.height());
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator+(const size<Rep>& lhs, const size<Rep>& rhs) {
  return { lhs.width() + rhs.width(), lhs.height() + rhs.height() };
}


template<typename Rep>
constexpr size<Rep>& operator-=(size<Rep>& lhs, const size<Rep>& rhs) {
  lhs.set(lhs.width() - rhs.width(), lhs.height() - rhs.height());
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator-(const size<Rep>& lhs, const size<Rep>& rhs) {
  return { lhs.width() - rhs.width(), lhs.height() - rhs.height() };
}


template<typename Rep, typename Rep2>
constexpr size<Rep>& operator*=(size<Rep>& lhs, Rep2 value) {
  lhs.set(static_cast<Rep>(lhs.width() * value), static_cast<Rep>(lhs.height() * value));
  return lhs;
}

template<typename Rep, typename Rep2>
constexpr size<Rep> operator*(const size<Rep>& lhs, Rep2 value) {
  return { static_cast<Rep>(lhs.width() * value), static_cast<Rep>(lhs.height() * value) };
}

template<typename Rep, typename Rep2>
constexpr size<Rep> operator*(Rep2 value, const size<Rep>& rhs) {
  return rhs * value;
}


template<typename Rep, typename Rep2>
constexpr size<Rep>& operator/=(size<Rep>& lhs, Rep2 value) {
  lhs.set(static_cast<Rep>(lhs.width() / value), static_cast<Rep>(lhs.height() / value));
  return lhs;
}

template<typename Rep, typename Rep2>
constexpr size<Rep> operator/(const size<Rep>& lhs, Rep2 value) {
  return { static_cast<Rep>(lhs.width() / value), static_cast<Rep>(lhs.height() / value) };
}


template<typename Rep>
constexpr bool operator==(const size<Rep>& lhs, const size<Rep>& rhs) {
  return lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

template<typename Rep>
constexpr bool operator!=(const size<Rep>& lhs, const size<Rep>& rhs) {
  return !(lhs == rhs);
}


using sizei = size<int>;
using sizef = size<float>;

}  // namepsace gfx