#pragma once

namespace gfx {

template<typename T>
struct point {
  using element_type = T;

  constexpr point();
  constexpr point(T x, T y);

  template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr point(const point<U>& other);

  constexpr T x() const { return x_; }
  constexpr T y() const { return y_; }

  constexpr void set(T new_x, T new_y);
  constexpr void set_x(T new_x);
  constexpr void set_y(T new_y);

private:
  T x_, y_;
};


template<typename T>
constexpr point<T>::point()
  : point(T(0), T(0)) {
}

template<typename T>
constexpr point<T>::point(T x, T y)
  : x_(x)
  , y_(y) {
}

template<typename T>
template<typename U, typename>
constexpr point<T>::point(const point<U>& other)
  : x_(other.x_)
  , y_(other.y_) {
}


template<typename T>
constexpr void point<T>::set(T new_x, T new_y) {
  x_ = new_x;
  y_ = new_y;
}

template<typename T>
constexpr void point<T>::set_x(T new_x) {
  x_ = new_x;
}

template<typename T>
constexpr void point<T>::set_y(T new_y) {
  y_ = new_y;
}


template<typename T>
constexpr point<T>& operator+=(point<T>& lhs, const point<T>& rhs) {
  lhs.set(lhs.x() + rhs.x(), lhs.y() + rhs.y());
  return lhs;
}

template<typename T>
constexpr point<T> operator+(const point<T>& lhs, const point<T>& rhs) {
  return { lhs.x() + rhs.x(), lhs.y() + rhs.y() };
}


template<typename T>
constexpr point<T>& operator-=(point<T>& lhs, const point<T>& rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.set(lhs.x() - rhs.x(), lhs.y() - rhs.y());
  return lhs;
}

template<typename T>
constexpr point<T> operator-(const point<T>& lhs, const point<T>& rhs) {
  return { lhs.x() - rhs.x(), lhs.y() - rhs.y() };
}

template<typename T>
constexpr point<T> operator-(const point<T>& pt) {
  return { -pt.x(), -pt.y() };
}


template<typename T, typename U>
constexpr point<T>& operator*=(point<T>& lhs, U value) {
  lhs.set(static_cast<T>(lhs.x() * value), static_cast<T>(lhs.y() * value));
  return lhs;
}

template<typename T, typename U>
constexpr point<T> operator*(const point<T>& lhs, U value) {
  return { static_cast<T>(lhs.x() * value), static_cast<T>(lhs.y() * value) };
}

template<typename T, typename U>
constexpr point<T> operator*(U value, const point<T>& rhs) {
  return rhs * value;
}


template<typename T, typename U>
constexpr point<T>& operator/=(point<T>& lhs, U value) {
  lhs.set(static_cast<T>(lhs.x() / value), static_cast<T>(lhs.y() / value));
  return lhs;
}

template<typename T, typename U>
constexpr point<T> operator/(const point<T>& lhs, U value) {
  return { static_cast<T>(lhs.x() / value), static_cast<T>(lhs.y() / value) };
}


template<typename T>
constexpr bool operator==(const point<T>& lhs, const point<T>& rhs) {
  return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

template<typename T>
constexpr bool operator!=(const point<T>& lhs, const point<T>& rhs) {
  return !(lhs == rhs);
}


using pointf = point<float>;
using pointi = point<int>;


constexpr float dot(const pointf& lhs, const pointf& rhs) {
  return lhs.x() * rhs.x() + lhs.y() * rhs.y();
}

constexpr float mag_squared(const pointf& pt) {
  return dot(pt, pt);
}

float mag(const pointf& pt);
float angle(const pointf& pt);
pointf to_unit(const pointf& pt);

// point on the ellipse of radii rx, ry corresponging to the angle,
// with positive values of angle indicating a clockwise rotation
pointf point_for_angle(float angle, float rx, float ry);

inline pointf point_for_angle(float angle, float radius = 1.f) {
  return point_for_angle(angle, radius, radius);
}

}  // namespace gfx