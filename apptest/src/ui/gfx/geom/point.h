#pragma once

namespace gfx {

template<typename Rep>
struct point {
  constexpr point();
  constexpr point(Rep x, Rep y);

  template<typename Rep2, typename = std::enable_if_t<std::is_convertible_v<Rep2, Rep>>>
  constexpr point(const point<Rep2>& other);

  void set(Rep new_x, Rep new_y);

  Rep x, y;
};


template<typename Rep>
constexpr point<Rep>::point()
  : point(Rep(0), Rep(0)) {
}

template<typename Rep>
constexpr point<Rep>::point(Rep x, Rep y)
  : x(x)
  , y(y) {
}

template<typename Rep>
template<typename Rep2, typename>
constexpr point<Rep>::point(const point<Rep2>& other)
  : x(other.x)
  , y(other.y) {
}


template<typename Rep>
void point<Rep>::set(Rep new_x, Rep new_y) {
  x = new_x;
  y = new_y;
}


template<typename Rep>
constexpr point<Rep>& operator+=(point<Rep>& lhs, const point<Rep>& rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

template<typename Rep>
constexpr point<Rep> operator+(point<Rep> lhs, const point<Rep>& rhs) {
  return lhs += rhs;
}


template<typename Rep>
constexpr point<Rep>& operator-=(point<Rep>& lhs, const point<Rep>& rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

template<typename Rep>
constexpr point<Rep> operator-(point<Rep> lhs, const point<Rep>& rhs) {
  return lhs -= rhs;
}

template<typename Rep>
constexpr point<Rep> operator-(const point<Rep>& pt) {
  return { -pt.x, -pt.y };
}


template<typename Rep, typename Rep2>
constexpr point<Rep>& operator*=(point<Rep>& lhs, Rep2 value) {
  lhs.x *= value;
  lhs.y *= value;
  return lhs;
}

template<typename Rep, typename Rep2>
constexpr point<Rep> operator*(point<Rep> lhs, Rep2 value) {
  return lhs *= value;
}

template<typename Rep, typename Rep2>
constexpr point<Rep> operator*(Rep2 value, const point<Rep>& rhs) {
  return rhs * value;
}


template<typename Rep, typename Rep2>
constexpr point<Rep>& operator/=(point<Rep>& lhs, Rep2 value) {
  lhs.x /= value;
  lhs.y /= value;
  return lhs;
}

template<typename Rep, typename Rep2>
constexpr point<Rep> operator/(point<Rep> lhs, Rep2 value) {
  return lhs /= value;
}


template<typename Rep>
constexpr bool operator==(const point<Rep>& lhs, const point<Rep>& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename Rep>
constexpr bool operator!=(const point<Rep>& lhs, const point<Rep>& rhs) {
  return !(lhs == rhs);
}


using pointf = point<float>;
using pointi = point<int>;


constexpr float dot(const pointf& lhs, const pointf& rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

constexpr float mag_squared(const pointf& pt) {
  return dot(pt, pt);
}

float mag(const pointf& pt);
float angle(const pointf& pt);
pointf to_unit(const pointf& pt);

pointf point_for_angle(float angle, float rx, float ry);

inline pointf point_for_angle(float angle, float radius = 1.f) {
  return point_for_angle(angle, radius, radius);
}

}  // namespace gfx