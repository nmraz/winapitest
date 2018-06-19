#pragma once

#include "ui/gfx/color.h"
#include <d2d1_1.h>

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

int bytes_per_pixel(pixel_format fmt);

color read_pixel(const void* pixel, pixel_format fmt, alpha_mode amode);
void write_pixel(void* pixel, const color& col, pixel_format fmt, alpha_mode amode);

}  // namespace gfx