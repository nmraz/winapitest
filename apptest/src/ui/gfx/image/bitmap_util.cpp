#include "bitmap_util.h"

namespace gfx {

int compute_pitch(int total_size, int height, pixel_format fmt) {
  ASSERT(total_size % height == 0) << "Invalid bitmap height";

  if (!total_size) {
    return 0;  // allow a height of 0 if there are no pixels
  }

  return total_size / height * bytes_per_pixel(fmt);
}

int pixel_offset(int x, int y, int pitch, pixel_format fmt) {
  return y * pitch + x * bytes_per_pixel(fmt);
}

}  // namespace gfx