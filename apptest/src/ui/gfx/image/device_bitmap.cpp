#include "device_bitmap.h"

namespace gfx {

device_bitmap::device_bitmap(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info)
  : device_image(std::move(dev))
  , d2d_bitmap_(std::move(d2d_bitmap))
  , info_(info) {
}

}  // namespac gfx