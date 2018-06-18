#include "bitmap.h"

#include "ui/gfx/util.h"

namespace gfx {

std::unique_ptr<bitmap> bitmap::create(const bitmap_info& info, const sizei& size,
  base::span<const std::byte> data) {
  return std::unique_ptr<bitmap>(new bitmap(info, size, data));
}


sizef bitmap::size() const {
  return {
    px_to_dip(pixel_size().width(), info().dpix()),
    px_to_dip(pixel_size().height(), info().dpiy())
  };
}

int bitmap::pitch() const {
  return compute_pitch(static_cast<int>(pixels().size()), pixel_size().height(), info().format());
}


// PRIVATE

bitmap::bitmap(const bitmap_info& info, const sizei& size, base::span<const std::byte> data)
  : pixels_(data.begin(), data.end())
  , pixel_size_(size)
  , info_(info) {
}

}  // namespace gfx