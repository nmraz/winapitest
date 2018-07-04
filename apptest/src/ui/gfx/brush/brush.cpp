#include "brush.h"

#include "ui/gfx/d2d/convs.h"

namespace gfx {

impl::d2d_brush_ptr brush::d2d_brush(impl::device_impl* dev) const {
  auto d2d_brush = do_get_d2d_brush(dev);
  
  d2d_brush->SetOpacity(opacity());
  d2d_brush->SetTransform(impl::mat33_to_d2d_mat32(transform()));
  
  return d2d_brush;
}

}  // namespace gfx