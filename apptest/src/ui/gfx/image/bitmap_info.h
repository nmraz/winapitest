#pragma once

#include <d2d1.h>

namespace gfx {

enum class pixel_format {
  unknown = DXGI_FORMAT_UNKNOWN,
  rgba8888 = DXGI_FORMAT_R8G8B8A8_UNORM,
  bgra8888 = DXGI_FORMAT_B8G8R8A8_UNORM
};

enum class alpha_mode {
  unknown = D2D1_ALPHA_MODE_UNKNOWN,
  premul = D2D1_ALPHA_MODE_PREMULTIPLIED,
  unpremul = D2D1_ALPHA_MODE_STRAIGHT,
  opaque = D2D1_ALPHA_MODE_IGNORE
};


class bitmap_info {
public:
  constexpr bitmap_info(pixel_format format = pixel_format::unknown,
    alpha_mode mode = alpha_mode::unknown, float dpix = 96, float dpiy = 96)
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