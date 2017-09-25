#include "color.h"

#include <algorithm>

namespace gfx {

color lerp(const color& from, const color& to, double t) {
  return {
    from.r() + (to.r() - from.r()) * t,
    from.g() + (to.g() - from.g()) * t,
    from.b() + (to.b() - from.b()) * t,
    from.a() + (to.a() - from.a()) * t
  };
}

color alpha_blend(const color& foreground, const color& background, float alpha) {
  if (alpha == 0.f) {
    return background;
  } else if (alpha == 1.f) {
    return foreground;
  }

  double norm = foreground.a() * alpha + background.a() * (1 - alpha);
  if (norm == 0.0) {
    return color{};
  }

  double fg_weight = foreground.a() * alpha / norm;
  double bg_weight = background.a() * alpha / norm;

  return {
    foreground.r() * fg_weight + background.r() * bg_weight,
    foreground.g() * fg_weight + background.g() * bg_weight,
    foreground.b() * fg_weight + background.b() * bg_weight,
    norm
  };
}

}  // namepsace gfx