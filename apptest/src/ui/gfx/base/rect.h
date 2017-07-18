#pragma once

#include "ui/gfx/base/point.h"
#include "ui/gfx/base/util.h"

namespace gfx {

template<typename Rep>
struct rect {
	constexpr rect();
	constexpr rect(Rep x, Rep y, Rep width, Rep height);
	constexpr rect(const point<Rep>& origin, Rep width, Rep height);

	constexpr point<Rep> origin() const;

	constexpr Rep area() const;
	constexpr bool empty() const;

	constexpr bool contains(const point<Rep>& pt) const;

	Rep x;
	Rep y;
	Rep width;
	Rep height;
};


template<typename Rep>
constexpr rect<Rep>::rect()
	: x(0)
	, y(0)
	, width(0)
	, height(0) {
}

template<typename Rep>
constexpr rect<Rep>::rect(Rep x, Rep y, Rep width, Rep height)
	: x(x)
	, y(y)
	, width(width)
	, height(height) {
}

template<typename Rep>
constexpr rect<Rep>::rect(const point<Rep>& origin, Rep width, Rep height)
	: rect(origin.x, origin.y, width, height) {
}


template<typename Rep>
constexpr point<Rep> rect<Rep>::origin() const {
	return {x, y};
}


template<typename Rep>
constexpr Rep rect<Rep>::area() const {
	return width * height;
}

template<typename Rep>
constexpr bool rect<Rep>::empty() const {
	return area() == Rep(0);
}


template<typename Rep>
constexpr bool rect<Rep>::contains(const point<Rep>& pt) const {
	return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
}


template<typename Rep>
constexpr bool operator==(const rect<Rep>& rhs, const rect<Rep>& lhs) {
	return rhs.x == lhs.x && rhs.y == lhs.y && rhs.width == lhs.width && rhs.height == lhs.height;
}

template<typename Rep>
constexpr bool operator!=(const rect<Rep>& rhs, const rect<Rep>& lhs) {
	return !(rhs == lhs);
}


template<typename Rep>
constexpr rect<Rep> lerp(const rect<Rep>& from, const rect<Rep> to, double t) {
	return {
		lerp(from.x, to.x, t),
		lerp(from.y, to.y, t),
		lerp(from.width, to.width, t),
		lerp(from.height, to.height, t)
	};
}


using rectf = rect<float>;
using recti = rect<int>;

}  // namespace gfx