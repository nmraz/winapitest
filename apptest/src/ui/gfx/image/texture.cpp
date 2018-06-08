#include "texture.h"

#include "ui/gfx/device_impl.h"
#include "ui/gfx/d2d/convs.h"

namespace gfx {

std::unique_ptr<texture> texture::create(device::ptr dev, const bitmap_info& info,
  const sizei& size, base::span<const std::byte> data) {
  auto* dev_impl = static_cast<impl::device_impl*>(dev.get());

  return std::unique_ptr<texture>(new texture(std::move(dev),
    dev_impl->create_bitmap(info, size, D2D1_BITMAP_OPTIONS_NONE, data), info));
}


sizef texture::size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetSize());
}

sizei texture::pixel_size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetPixelSize());
}


texture::texture(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info)
  : device_image(std::move(dev))
  , d2d_bitmap_(std::move(d2d_bitmap))
  , info_(info) {
}

}  // namespac gfx