#include "bitmap_lock.h"

#include "ui/gfx/image/bitmap.h"

namespace gfx {

bitmap_lock::~bitmap_lock() {
  bmp_.unlock();
}

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


base::span<std::byte> bitmap_lock::pixels() {
  return bmp_.pixels_;
}

base::span<const std::byte> bitmap_lock::pixels() const {
  return bmp_.pixels_;
}


// PRIVATE

bitmap_lock::bitmap_lock(bitmap& bmp)
  : bmp_(bmp) {
}

}  // namespace gfx