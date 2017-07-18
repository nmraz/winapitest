#pragma once

namespace gfx {

template<typename Rep>
struct point {
	Rep x, y;
};


template<typename Rep>
constexpr point<Rep>& operator+=(point<Rep>& rhs, const point<Rep>& lhs) {
	rhs.x += lhs.x;
	rhs.y += lhs.y;
	return rhs;
}

template<typename Rep>
constexpr point<Rep> operator+(point<Rep> rhs, const point<Rep>& lhs) {
	return rhs += lhs;
}


template<typename Rep>
constexpr point<Rep>& operator-=(point<Rep>& rhs, const point<Rep>& lhs) {
	rhs.x -= lhs.x;
	rhs.y -= lhs.y;
	return rhs;
}

template<typename Rep>
constexpr point<Rep> operator-(point<Rep> rhs, const point<Rep>& lhs) {
	return rhs -= lhs;
}


template<typename Rep>
constexpr point<Rep>& operator*=(point<Rep>& rhs, Rep value) {
	rhs.x *= value;
	rhs.y *= value;
	return rhs;
}

template<typename Rep>
constexpr point<Rep> operator*(point<Rep> rhs, Rep value) {
	return rhs *= value;
}

template<typename Rep>
constexpr point<Rep> operator*(Rep value, const point<Rep>& lhs) {
	return lhs * value;
}


template<typename Rep>
constexpr point<Rep>& operator/=(point<Rep>& rhs, Rep value) {
	rhs.x /= value;
	rhs.y /= value;
	return rhs;
}

template<typename Rep>
constexpr point<Rep> operator/(point<Rep> rhs, Rep value) {
	return rhs /= value;
}

template<typename Rep>
constexpr point<Rep> operator/(Rep value, const point<Rep>& lhs) {
	return lhs / value;
}


template<typename Rep>
constexpr bool operator==(const point<Rep>& rhs, const point<Rep>& lhs) {
	return rhs.x == lhs.x && rhs.y == lhs.y;
}

template<typename Rep>
constexpr bool operator!=(const point<Rep>& rhs, const point<Rep>& lhs) {
	return !(rhs == lhs);
}


using pointf = point<float>;
using pointi = point<int>;

}  // namespace gfx