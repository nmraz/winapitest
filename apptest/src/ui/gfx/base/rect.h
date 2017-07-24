#pragma once

#include "ui/gfx/base/point.h"
#include "ui/gfx/base/size.h"
#include "ui/gfx/base/util.h"
#include <type_traits>

namespace gfx {

template<typename Rep>
struct rect {
	static constexpr struct by_xywh_tag {} by_xywh;
	static constexpr struct by_bounds_tag {} by_bounds;

	constexpr rect();
	constexpr rect(const point<Rep>& origin, const size<Rep>& size);

	constexpr rect(by_xywh_tag, Rep x, Rep y, Rep width, Rep height);
	constexpr rect(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right);

	template<typename Rep2, typename = std::enable_if_t<std::is_convertible_v<Rep2, Rep>>>
	constexpr rect(const rect<Rep2>& other);

	constexpr Rep x() const { return origin.x; }
	constexpr Rep y() const { return origin.y; }
	constexpr Rep width() const { return sz.width; }
	constexpr Rep height() const { return sz.height; }

	constexpr Rep right() const { return x() + width(); }
	constexpr Rep bottom() const { return y() + height(); }

	constexpr point<Rep> top_right() const { return {right(), y()}; }
	constexpr point<Rep> bottom_right() const { return {right(), bottom()}; }
	constexpr point<Rep> bottom_left() const { return {x(), bottom()}; }

	void set(by_xywh_tag, Rep x, Rep y, Rep width, Rep height);
	void set(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right);

	constexpr Rep area() const { return sz.area(); }
	constexpr bool empty() const { return sz.empty(); }

	constexpr bool contains(const point<Rep>& pt) const;

	point<Rep> origin;
	size<Rep> sz;
};


template<typename Rep>
constexpr rect<Rep>::rect()
	: rect(0, 0, 0, 0) {
}

template<typename Rep>
constexpr rect<Rep>::rect(const point<Rep>& origin, const size<Rep>& sz)
	: origin(origin)
	, size(sz) {
}

template<typename Rep>
constexpr rect<Rep>::rect(by_xywh_tag, Rep x, Rep y, Rep width, Rep height) {
	set(by_xywh, x, y, width, height);
}

template<typename Rep>
constexpr rect<Rep>::rect(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right) {
	set(by_bounds, top, left, bottom, right);
}

template<typename Rep>
template<typename Rep2, typename>
constexpr rect<Rep>::rect(const rect<Rep2>& other)
	: origin(other.origin)
	, height(other.height) {
}


template<typename Rep>
void rect<Rep>::set(by_xywh_tag, Rep x, Rep y, Rep width, Rep height) {
	origin.set(x, y);
	sz.set(width, height);
}

template<typename Rep>
void rect<Rep>::set(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right) {
	if (bottom < top) {
		std::swap(bottom, top);
	}
	if (right < left) {
		std::swap(right, left);
	}
	set(by_xywh, top, left, right - left, bottom - top);
}


template<typename Rep>
constexpr bool rect<Rep>::contains(const point<Rep>& pt) const {
	return x() <= pt.x && pt.x < right() && y() <= pt.y && pt.y < bottom();
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