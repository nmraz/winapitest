#include "transform.h"

#include <cmath>

namespace gfx::transform {

mat33f translate(float dx, float dy) {
  return {
    1,  0,  0,
    0,  1,  0,
    dx, dy, 1
  };
}

mat33f scale(float sx, float sy) {
  return {
    sx, 0,  0,
    0,  sy, 0,
    0,  0,  1
  };
}

mat33f shear(float shx, float shy) {
  return {
    1,   shy, 0,
    shx, 1,   0,
    0,   0,   1
  };
}

mat33f rotate(float theta) {
  float sin_theta = std::sin(theta);
  float cos_theta = std::cos(theta);

  return {
    cos_theta,  sin_theta,  0,
    -sin_theta, cos_theta,  0,
    0,          0,          1
  };
}

mat33f centered_about(const mat33f& tform, const pointf& center) {
  // translate `center` to origin, apply transform, and translate back
  return translate(-center.x, -center.y) * tform * translate(center.x, center.y);
}


pointf apply(const mat33f& tform, const pointf& pt) {
  // postmultiply `pt` by `tform`

  return {
    pt.x * tform.get(0, 0) + pt.y * tform.get(1, 0) + tform.get(2, 0),
    pt.x * tform.get(0, 1) + pt.y * tform.get(1, 1) + tform.get(2, 1)
  };
}

}  // namespace gfx::transform