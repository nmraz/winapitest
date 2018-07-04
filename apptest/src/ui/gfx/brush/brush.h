#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/matrix.h"

namespace gfx {
namespace impl {

class device_impl;

}  // namespace impl


class brush : public base::non_copy_movable {
public:
  virtual ~brush() {}

  const mat33f& transform() const;
  void set_transform(const mat33f& tform);

  float opacity() const;
  void set_opacity(float opacity);

  impl::d2d_brush_ptr d2d_brush(impl::device_impl* dev) const;

protected:
  virtual impl::d2d_brush_ptr do_get_d2d_brush(impl::device_impl* dev) const = 0;
};

}  // namespace gfx