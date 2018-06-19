#pragma once

#include "base/assert.h"
#include "ui/gfx/pixel_format.h"

namespace gfx {

int compute_pitch(int total_size, int height, pixel_format fmt);
int pixel_offset(int x, int y, int pitch, pixel_format fmt);

}  // namespace gfx