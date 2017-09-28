#pragma once

#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/geom/size.h"
#include <array>

namespace gfx {

enum class rect_corner {
  top_left = 0,
  top_right = 1,
  bottom_right = 2,
  bottom_left = 3
};


class round_rect {
public:
  round_rect() = default;

  round_rect(const rectf& bounds, const sizef& radius);
  round_rect(const rectf& bounds, const sizef& top_left, const sizef& top_right,
    const sizef& bottom_right, const sizef& bottom_left);

  const rectf& bounds() const { return bounds_; }
  rectf& bounds() { return bounds_; }

  const sizef& radius(rect_corner corner) const;
  void set_radius(rect_corner corner, const sizef& radius);

  bool contains(const pointf& pt);

private:
  void scale_radii();
  friend bool operator==(const round_rect& lhs, const round_rect& rhs);

  rectf bounds_;
  // stored as: top-left, top-right, bottom-right, bottom-left
  std::array<sizef, 4> radii_;
};


inline bool operator==(const round_rect& lhs, const round_rect& rhs) {
  return lhs.bounds_ == rhs.bounds_ && lhs.radii_ == rhs.radii_;
}

inline bool operator!=(const round_rect& lhs, const round_rect& rhs) {
  return !(lhs == rhs);
}

}  // namespace gfx