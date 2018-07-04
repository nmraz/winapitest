#include "image_brush.h"

namespace gfx {

std::unique_ptr<image_brush> image_brush::create() {
  return std::unique_ptr<image_brush>(new image_brush());
}


// PRIVATE

impl::d2d_brush_ptr image_brush::do_get_d2d_brush(impl::device_impl* dev) const {
  return nullptr;
}

}  // namespace gfx