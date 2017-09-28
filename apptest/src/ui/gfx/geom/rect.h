#pragma once

#include "base/assert.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/util.h"
#include <algorithm>
#include <type_traits>

namespace gfx {

template<typename Rep>
class rect {
public:
  static constexpr struct by_xywh_tag {} by_xywh{};
  static constexpr struct by_bounds_tag {} by_bounds{};

  constexpr rect();
  constexpr rect(const point<Rep>& origin, const size<Rep>& size);

  constexpr rect(by_xywh_tag, Rep x, Rep y, Rep width, Rep height);
  constexpr rect(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right);

  template<typename Rep2, typename = std::enable_if_t<std::is_convertible_v<Rep2, Rep>>>
  constexpr rect(const rect<Rep2>& other);


  constexpr Rep x() const { return origin_.x; }
  constexpr void set_x(Rep x) { origin_.x = x; }

  constexpr Rep y() const { return origin_.y; }
  constexpr void set_y(Rep y) { origin_.y = y; }

  constexpr Rep width() const { return size_.width(); }
  constexpr void set_width(Rep new_width) { size_.set_width(new_width); }

  constexpr Rep height() const { return size_.height(); }
  constexpr void set_height(Rep new_height) { size_.set_height(new_height); }


  constexpr const size<Rep>& get_size() const { return size_; }
  constexpr void set_size(const size<Rep>& new_size) { size_ = new_size; }

  constexpr const point<Rep>& origin() const { return origin_; }
  constexpr void set_origin(const point<Rep> new_origin) { origin_ = new_origin; }


  constexpr Rep right() const { return x() + width(); }
  constexpr Rep bottom() const { return y() + height(); }

  constexpr point<Rep> top_right() const { return {right(), y()}; }
  constexpr point<Rep> bottom_right() const { return {right(), bottom()}; }
  constexpr point<Rep> bottom_left() const { return {x(), bottom()}; }
  constexpr point<Rep> center() const { return { x() + width() / 2, y() + height() / 2 }; }

  constexpr void set_xywh(Rep x, Rep y, Rep width, Rep height);
  constexpr void set_bounds(Rep top, Rep left, Rep bottom, Rep right);

  constexpr Rep area() const { return size_.area(); }
  constexpr bool empty() const { return size_.empty(); }

  constexpr bool contains(const point<Rep>& pt) const;

  constexpr bool intersects(const rect& other) const;
  constexpr void intersect(const rect& other);

private:
  point<Rep> origin_;
  size<Rep> size_;
};


template<typename Rep>
constexpr rect<Rep>::rect()
  : rect(0, 0, 0, 0) {
}

template<typename Rep>
constexpr rect<Rep>::rect(const point<Rep>& origin, const size<Rep>& sz)
  : origin_(origin)
  , size_(sz) {
}

template<typename Rep>
constexpr rect<Rep>::rect(by_xywh_tag, Rep x, Rep y, Rep width, Rep height) {
  set_xywh(x, y, width, height);
}

template<typename Rep>
constexpr rect<Rep>::rect(by_bounds_tag, Rep top, Rep left, Rep bottom, Rep right) {
  set_bounds(top, left, bottom, right);
}

template<typename Rep>
template<typename Rep2, typename>
constexpr rect<Rep>::rect(const rect<Rep2>& other)
  : origin_(other.origin_)
  , size_(other.size_) {
}


template<typename Rep>
constexpr void rect<Rep>::set_xywh(Rep x, Rep y, Rep width, Rep height) {
  origin_.set(x, y);
  size_.set(width, height);
}

template<typename Rep>
constexpr void rect<Rep>::set_bounds(Rep top, Rep left, Rep bottom, Rep right) {
  if (bottom < top) {
    std::swap(bottom, top);
  }
  if (right < left) {
    std::swap(right, left);
  }
  set_xywh(top, left, right - left, bottom - top);
}


template<typename Rep>
constexpr bool rect<Rep>::contains(const point<Rep>& pt) const {
  return x() <= pt.x && pt.x < right() && y() <= pt.y && pt.y < bottom();
}


template<typename Rep>
constexpr bool rect<Rep>::intersects(const rect& other) const {
  if (empty() || other.empty()) {
    return false;
  }

  return x() < other.right() && other.x() < right() && y() < other.bottom() && other.y() < bottom();
}

template<typename Rep>
constexpr void rect<Rep>::intersect(const rect& other) {
  if (empty() || other.empty()) {
    set_xywh(0, 0, 0, 0);
    return;
  }
  
  Rep new_left = std::max(x(), other.x());
  Rep new_top = std::max(y(), other.y());
  Rep new_right = std::min(right(), other.right());
  Rep new_bottom = std::min(bottom(), other.bottom());

  if (new_left >= new_right || new_top >= new_bottom) {  // no intersection
    set_xywh(0, 0, 0, 0);
    return;
  }

  set_bounds(new_top, new_left, new_bottom, new_right);
}


template<typename Rep>
constexpr bool operator==(const rect<Rep>& lhs, const rect<Rep>& rhs) {
  return lhs.origin() == rhs.origin() && lhs.get_size() == rhs.get_size();
}

template<typename Rep>
constexpr bool operator!=(const rect<Rep>& lhs, const rect<Rep>& rhs) {
  return !(rhs == lhs);
}


using rectf = rect<float>;
using recti = rect<int>;

}  // namespace gfx