#include "transform.h"

#include <cmath>

namespace gfx::transform {

mat33f rotate(float theta) {
  float sin_theta = std::sin(theta);
  float cos_theta = std::cos(theta);

  return {
    cos_theta,  sin_theta,  0,
    -sin_theta, cos_theta,  0,
    0,          0,          1
  };
}

}  // namespace gfx::transform