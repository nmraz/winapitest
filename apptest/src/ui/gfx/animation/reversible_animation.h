#pragma once

#include "ui/gfx/animation/animation.h"
#include "base/function.h"
#include <algorithm>

namespace gfx {

class reversible_animation {
  using anim_type = animation<double>;

public:
  using easing_func = anim_type::easing_func;
  using progress_callback = base::function<void(reversible_animation&)>;

  using duration_type = anim_type::duration_type;

  reversible_animation(easing_func func, progress_callback callback);

  void set_duration(duration_type duration) { duration_ = duration; }
  duration_type duration() const { return duration_; }

  bool is_running() const { return anim_.is_running(); }

  void set(double prog) { anim_.set(std::clamp(prog, 0.0, 1.0)); }
  void animate_to(double prog);

  void enter() { animate_to(1.0); }
  void leave() { animate_to(0.0); }

  void finish() { set(1.0); }
  void reset() { set(0.0); }

  double prog() const { return anim_.val(); }

private:
  anim_type anim_;
  duration_type duration_;
};

}  // namespace gfx