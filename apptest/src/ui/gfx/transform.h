#pragma once

#include "ui/gfx/geom/matrix.h"
#include "ui/gfx/geom/point.h"

namespace gfx::transform {

// For consistency with Direct2D, points are considered row vectors
// and are postmultiplied by the transforms themeselves. This also
// provides a more intuitive syntax, where `A * B` means "first apply
// A, followed by B".

constexpr mat33f identity() {
  return {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
  };
}

constexpr mat33f translate(float dx, float dy) {
  return {
    1,  0,  0,
    0,  1,  0,
    dx, dy, 1
  };
}

constexpr mat33f scale(float sx, float sy) {
  return {
    sx, 0,  0,
    0,  sy, 0,
    0,  0,  1
  };
}

constexpr mat33f shear(float shx, float shy) {
  return {
    1,   shy, 0,
    shx, 1,   0,
    0,   0,   1
  };
}

mat33f rotate(float theta);

constexpr mat33f centered_about(const mat33f& tform, const pointf& center) {
  // translate `center` to origin, apply transform, and translate back
  return translate(-center.x, -center.y) * tform * translate(center.x, center.y);
}


constexpr pointf apply(const mat33f& tform, const pointf& pt) {
  // postmultiply `pt` by `tform`

  return {
    pt.x * tform.get(0, 0) + pt.y * tform.get(1, 0) + tform.get(2, 0),
    pt.x * tform.get(0, 1) + pt.y * tform.get(1, 1) + tform.get(2, 1)
  };
}

}  // namespace gfx::transform