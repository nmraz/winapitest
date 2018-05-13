#pragma once

#include "base/assert.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/util.h"
#include <algorithm>
#include <type_traits>

namespace gfx {

template<typename T>
class rect {
public:
  static constexpr struct by_xywh_tag {} by_xywh{};
  static constexpr struct by_bounds_tag {} by_bounds{};

  constexpr rect() = default;
  constexpr rect(const point<T>& origin, const size<T>& size);

  constexpr rect(by_xywh_tag, T x, T y, T width, T height);
  constexpr rect(by_bounds_tag, T top, T left, T bottom, T right);

  template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
  constexpr rect(const rect<U>& other);


  constexpr T x() const { return origin_.x; }
  constexpr void set_x(T x) { origin_.x = x; }

  constexpr T y() const { return origin_.y; }
  constexpr void set_y(T y) { origin_.y = y; }

  constexpr T width() const { return size_.width(); }
  constexpr void set_width(T new_width) { size_.set_width(new_width); }

  constexpr T height() const { return size_.height(); }
  constexpr void set_height(T new_height) { size_.set_height(new_height); }


  constexpr const size<T>& get_size() const { return size_; }
  constexpr void set_size(const size<T>& new_size) { size_ = new_size; }

  constexpr const point<T>& origin() const { return origin_; }
  constexpr void set_origin(const point<T> new_origin) { origin_ = new_origin; }


  constexpr T right() const { return x() + width(); }
  constexpr T bottom() const { return y() + height(); }

  constexpr point<T> top_right() const { return { right(), y() }; }
  constexpr point<T> bottom_right() const { return { right(), bottom() }; }
  constexpr point<T> bottom_left() const { return { x(), bottom() }; }
  constexpr point<T> center() const { return { x() + width() / 2, y() + height() / 2 }; }

  constexpr void set_xywh(T x, T y, T width, T height);
  constexpr void set_bounds(T top, T left, T bottom, T right);

  constexpr T area() const { return size_.area(); }
  constexpr bool empty() const { return size_.empty(); }

  constexpr bool contains(const point<T>& pt) const;

  constexpr bool intersects(const rect& other) const;
  constexpr void intersect(const rect& other);

private:
  point<T> origin_{};
  size<T> size_{};
};


template<typename T>
constexpr rect<T>::rect(const point<T>& origin, const size<T>& sz)
  : origin_(origin)
  , size_(sz) {
}

template<typename T>
constexpr rect<T>::rect(by_xywh_tag, T x, T y, T width, T height) {
  set_xywh(x, y, width, height);
}

template<typename T>
constexpr rect<T>::rect(by_bounds_tag, T top, T left, T bottom, T right) {
  set_bounds(top, left, bottom, right);
}

template<typename T>
template<typename U, typename>
constexpr rect<T>::rect(const rect<U>& other)
  : origin_(other.origin_)
  , size_(other.size_) {
}


template<typename T>
constexpr void rect<T>::set_xywh(T x, T y, T width, T height) {
  origin_.set(x, y);
  size_.set(width, height);
}

template<typename T>
constexpr void rect<T>::set_bounds(T top, T left, T bottom, T right) {
  if (bottom < top) {
    std::swap(bottom, top);
  }
  if (right < left) {
    std::swap(right, left);
  }
  set_xywh(top, left, right - left, bottom - top);
}


template<typename T>
constexpr bool rect<T>::contains(const point<T>& pt) const {
  return x() <= pt.x && pt.x < right() && y() <= pt.y && pt.y < bottom();
}


template<typename T>
constexpr bool rect<T>::intersects(const rect& other) const {
  if (empty() || other.empty()) {
    return false;
  }

  return x() < other.right() && other.x() < right() && y() < other.bottom() && other.y() < bottom();
}

template<typename T>
constexpr void rect<T>::intersect(const rect& other) {
  if (empty() || other.empty()) {
    set_xywh(0, 0, 0, 0);
    return;
  }
  
  T new_left = std::max(x(), other.x());
  T new_top = std::max(y(), other.y());
  T new_right = std::min(right(), other.right());
  T new_bottom = std::min(bottom(), other.bottom());

  if (new_left >= new_right || new_top >= new_bottom) {  // no intersection
    set_xywh(0, 0, 0, 0);
    return;
  }

  set_bounds(new_top, new_left, new_bottom, new_right);
}


template<typename T>
constexpr bool operator==(const rect<T>& lhs, const rect<T>& rhs) {
  return lhs.origin() == rhs.origin() && lhs.get_size() == rhs.get_size();
}

template<typename T>
constexpr bool operator!=(const rect<T>& lhs, const rect<T>& rhs) {
  return !(rhs == lhs);
}


template<typename T>
constexpr rect<T> lerp(const rect<T>& from, const rect<T>& to, double t) {
  return { lerp(from.origin(), to.origin(), t), lerp(from.get_size(), to.get_size(), t) };
}


using rectf = rect<float>;
using recti = rect<int>;

}  // namespace gfx