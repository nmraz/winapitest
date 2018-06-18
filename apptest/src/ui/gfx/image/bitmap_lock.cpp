#include "bitmap_lock.h"

#include "ui/gfx/image/bitmap.h"

namespace gfx {

const bitmap_info& bitmap_lock::info() const {
  return bmp_.info();
}


sizef bitmap_lock::size() const {
  return bmp_.size();
}

sizei bitmap_lock::pixel_size() const {
  return bmp_.pixel_size();
}

int bitmap_lock::pitch() const {
  return bmp_.pitch();
}

}  // namespace gfx