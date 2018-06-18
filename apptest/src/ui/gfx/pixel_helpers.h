#pragma once

#include "ui/gfx/color.h"
#include "ui/gfx/image/bitmap_info.h"

namespace gfx {

color read_pixel(const void* pixel, pixel_format fmt, alpha_mode amode);
void write_pixel(void* pixel, const color& col, pixel_format fmt, alpha_mode amode);

}  // namespace gfx