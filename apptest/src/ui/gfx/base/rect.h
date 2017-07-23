#pragma once

#include "ui/gfx/base/point.h"
#include "ui/gfx/base/size.h"
#include "ui/gfx/base/util.h"
#include <type_traits>

namespace gfx {

template<typename Rep>
struct rect {
	constexpr rect();
	constexpr rect(Rep x, Rep y, Rep width, Rep height);
	constexpr rect(const point<Rep>& origin, const size<Rep>& size);

	template<typename Rep2, typename = std::enable_if_t<std::is_convertible_v<Rep2, Rep>>>
	constexpr rect(const rect<Rep2>& other);

	constexpr Rep x() const { return origin.x; }
	constexpr Rep y() const { return origin.y; }
	constexpr Rep width() const { return sz.width; }
	constexpr Rep height() const { return sz.height; }

	void set(Rep x, Rep y, Rep width, Rep height);

	constexpr Rep area() const { return sz.area(); }
	constexpr bool empty() const { return sz.empty(); }

	constexpr bool fill_contains(const point<Rep>& pt) const;

	point<Rep> origin;
	size<Rep> sz;
};


template<typename Rep>
constexpr rect<Rep>::rect()
	: rect(0, 0, 0, 0) {
}

template<typename Rep>
constexpr rect<Rep>::rect(Rep x, Rep y, Rep width, Rep height)
	: rect({x, y}, {width, height}) {
}

template<typename Rep>
constexpr rect<Rep>::rect(const point<Rep>& origin, const size<Rep>& sz)
	: origin(origin)
	, size(sz) {
}

template<typename Rep>
template<typename Rep2, typename>
constexpr rect<Rep>::rect(const rect<Rep2>& other)
	: origin(other.origin)
	, height(other.height) {
}


template<typename Rep>
void rect<Rep>::set(Rep x, Rep y, Rep width, Rep height) {
	origin = {x, y};
	sz = {width, height};
}


template<typename Rep>
constexpr bool rect<Rep>::fill_contains(const point<Rep>& pt) const {
	return x < pt.x && pt.x < x + width && y < pt.y && pt.y < y + height;
}


template<typename Rep>
constexpr bool operator==(const rect<Rep>& rhs, const rect<Rep>& lhs) {
	return rhs.x == lhs.x && rhs.y == lhs.y && rhs.width == lhs.width && rhs.height == lhs.height;
}

template<typename Rep>
constexpr bool operator!=(const rect<Rep>& rhs, const rect<Rep>& lhs) {
	return !(rhs == lhs);
}


using rectf = rect<float>;
using recti = rect<int>;

}  // namespace gfx