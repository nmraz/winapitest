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

  constexpr Rep area() const { return width * height; }
  constexpr bool empty() const;

  void set(Rep new_width, Rep new_height);

  void expand_to(const size& other);
  void shrink_to(const size& other);

  Rep width, height;
};


template<typename Rep>
constexpr size<Rep>::size()
  : size(Rep(0), Rep(0)) {
}

template<typename Rep>
constexpr size<Rep>::size(Rep width, Rep height)
  : width(width)
  , height(height) {
}


template<typename Rep>
template<typename Rep2, typename>
constexpr size<Rep>::size(const size<Rep2>& other)
  : width(other.width)
  , height(other.height) {
}


template<typename Rep>
constexpr bool size<Rep>::empty() const {
  return width == 0 || height == 0;
}


template<typename Rep>
void size<Rep>::set(Rep new_width, Rep new_height) {
  width = new_width;
  height = new_height;
}


template<typename Rep>
void size<Rep>::expand_to(const size& other) {
  width = std::max(width, other.width);
  height = std::max(height, other.height);
}

template<typename Rep>
void size<Rep>::shrink_to(const size& other) {
  width = std::min(width, other.width);
  height = std::min(height, other.height);
}


template<typename Rep>
constexpr size<Rep>& operator+=(size<Rep>& rhs, const size<Rep>& lhs) {
  rhs.x += lhs.x;
  rhs.y += lhs.y;
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator+(size<Rep> rhs, const size<Rep>& lhs) {
  return rhs += lhs;
}


template<typename Rep>
constexpr size<Rep>& operator-=(size<Rep>& rhs, const size<Rep>& lhs) {
  rhs.x -= lhs.x;
  rhs.y -= lhs.y;
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator-(size<Rep> rhs, const size<Rep>& lhs) {
  return rhs -= lhs;
}


template<typename Rep>
constexpr size<Rep>& operator*=(size<Rep>& rhs, Rep value) {
  rhs.x *= value;
  rhs.y *= value;
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator*(size<Rep> rhs, Rep value) {
  return rhs *= value;
}

template<typename Rep>
constexpr size<Rep> operator*(Rep value, const size<Rep>& lhs) {
  return lhs * value;
}


template<typename Rep>
constexpr size<Rep>& operator/=(size<Rep>& rhs, Rep value) {
  rhs.x /= value;
  rhs.y /= value;
  return rhs;
}

template<typename Rep>
constexpr size<Rep> operator/(size<Rep> rhs, Rep value) {
  return rhs /= value;
}

template<typename Rep>
constexpr size<Rep> operator/(Rep value, const size<Rep>& lhs) {
  return lhs / value;
}


template<typename Rep>
constexpr bool operator==(const size<Rep>& lhs, const size<Rep>& rhs) {
  return lhs.width == rhs.width && lhs.height == rhs.height;
}

template<typename Rep>
constexpr bool operator!=(const size<Rep>& lhs, const size<Rep>& rhs) {
  return !(lhs == rhs);
}


using sizei = size<int>;
using sizef = size<float>;

}  // namepsace gfx