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
constexpr bool operator==(const point<Rep>& lhs, const point<Rep>& rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename Rep>
constexpr bool operator!=(const point<Rep>& lhs, const point<Rep>& rhs) {
	return !(lhs == rhs);
}


using pointf = point<float>;
using pointi = point<int>;

}  // namespace gfx