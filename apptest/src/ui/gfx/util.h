#pragma once

namespace gfx {

template<typename T>
constexpr T pi = T(3.141592653589);


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