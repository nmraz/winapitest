#include "texture.h"

#include "ui/gfx/d2d/convs.h"

namespace gfx {

texture::texture(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info)
  : device_image(std::move(dev))
  , d2d_bitmap_(std::move(d2d_bitmap))
  , info_(info) {
}


sizef texture::size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetSize());
}

sizei texture::pixel_size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetPixelSize());
}

}  // namespac gfx