#include "bitmap.h"

#include "ui/gfx/d2d/cached_d2d_resource.h"
#include "ui/gfx/device_impl.h"
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


impl::d2d_image_ptr bitmap::d2d_image(impl::device_impl* dev) const {
  return dev->cache().find_or_create(&key_, [&] {
    auto d2d_bitmap = dev->create_bitmap(info(), pixel_size(), D2D1_BITMAP_OPTIONS_NONE, pixels());
    return std::make_unique<impl::cached_d2d_resource<ID2D1Image>>(key_.version(), std::move(d2d_bitmap));
  })->resource();
}


// PRIVATE

bitmap::bitmap(const bitmap_info& info, const sizei& size, base::span<const std::byte> data)
  : pixels_(data.begin(), data.end())
  , pixel_size_(size)
  , info_(info) {
}

}  // namespace gfx