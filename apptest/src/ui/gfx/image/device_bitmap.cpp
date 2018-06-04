#include "device_bitmap.h"

#include "ui/gfx/d2d/convs.h"

namespace gfx {

device_bitmap::device_bitmap(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info)
  : device_image(std::move(dev))
  , d2d_bitmap_(std::move(d2d_bitmap))
  , info_(info) {
}


sizef device_bitmap::size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetSize());
}

sizei device_bitmap::pixel_size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetPixelSize());
}

}  // namespac gfx