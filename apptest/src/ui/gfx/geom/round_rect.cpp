#include "round_rect.h"

namespace gfx {

round_rect::round_rect(const rectf& bounds, const sizef& radius)
  : bounds_(bounds)
  , radius_(radius) {
}

void round_rect::set_radius(sizef radius) {
  radius.shrink_to(bounds_.get_size() / 2.f);
}

}  // namespace gfx