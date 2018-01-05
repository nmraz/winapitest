#include "reversible_animation.h"

#include <cmath>

namespace gfx {

reversible_animation::reversible_animation(easing_func easing, progress_callback callback)
  : anim_(
      std::move(easing),
      [this, callback = std::move(callback)](anim_type&) mutable { callback(*this); }
  ) {
}

void reversible_animation::animate_to(double prog) {
  prog = std::clamp(prog, 0.0, 1.0);
  anim_.set_duration(duration_ * std::abs(prog - anim_.val()));  // adjust time dynamically based on remaining distance
  anim_.animate_to(prog);
}

}  // namespace gfx