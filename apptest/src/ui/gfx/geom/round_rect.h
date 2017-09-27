#pragma once

#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/geom/size.h"

namespace gfx {

class round_rect {
public:
  round_rect() = default;
  round_rect(const rectf& bounds, const sizef& radius);

  const rectf& bounds() const { return bounds_; }
  rectf& bounds() { return bounds_; }

  const sizef& radius() const { return radius_; }
  void set_radius(sizef radius);

private:
  rectf bounds_;
  sizef radius_;
};

}  // namespace gfx