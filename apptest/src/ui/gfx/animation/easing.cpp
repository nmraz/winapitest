#include "easing.h"

#include "ui/gfx/util.h"
#include <cmath>

namespace gfx::easing {

double linear(double p) {
  return p;
}

double ease(double p) {
  return std::sin(p * half_pi<double>);
}

double ease_in(double p) {
  return p * p;
}

double ease_out(double p) {
  return 1 - ease_in(1 - p);
}

double ease_in_out(double p) {
  if (p < 0.5) {
    return ease_in(2 * p) / 2;
  } else {
    return 1 - ease_in(2 * p) / 2;
  }
}


}  // namespace gfx::easing