#pragma once

#include "ui/gfx/pixel_format.h"
#include "ui/gfx/util.h"

namespace gfx {

class bitmap_info {
public:
  constexpr bitmap_info(pixel_format format = pixel_format::unknown,
    alpha_mode mode = alpha_mode::unknown, float dpix = default_dpi, float dpiy = default_dpi)
    : format_(format)
    , alpha_(mode)
    , dpix_(dpix)
    , dpiy_(dpiy) { }

  constexpr pixel_format format() const { return format_; }
  constexpr alpha_mode alpha() const { return alpha_; }
  
  constexpr float dpix() const { return dpix_; }
  constexpr float dpiy() const { return dpiy_; }

private:
  pixel_format format_ ;
  alpha_mode alpha_;
  float dpix_;
  float dpiy_;
};

}  // namespace gfx