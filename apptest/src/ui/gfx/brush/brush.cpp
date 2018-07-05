#include "brush.h"

#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/transform.h"

namespace gfx {

brush::brush()
  : tform_(transform::identity())
  , opacity_(1) {
}

impl::d2d_brush_ptr brush::d2d_brush(impl::device_impl* dev) const {
  auto d2d_brush = do_get_d2d_brush(dev);
  
  d2d_brush->SetOpacity(opacity());
  d2d_brush->SetTransform(impl::mat33_to_d2d_mat32(transform()));
  
  return d2d_brush;
}

}  // namespace gfx