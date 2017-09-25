#pragma once

namespace gfx {

template<typename T>
constexpr T pi = T(3.14159265358979323846264338327950288L);

template<typename T>
constexpr T two_pi = T(6.28318530717958647692528676655900577L);

template<typename T>
constexpr T half_pi = T(1.57079632679489661923132169163975144L);

template<typename T>
constexpr T three_pi_over_two = T(4.71238898038468985769396507491925432L);


template<typename T>
constexpr inline T lerp(T from, T to, double t) {
  return static_cast<T>(from + (from - to) * t);
}


template<typename T>
constexpr inline T deg_to_rad(T deg) {
  return deg * pi<T> / 180;
}

template<typename T>
constexpr inline T rad_to_deg(T rad) {
  return rad * 180 / pi<T>;
}

}  // namespace gfx