#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/d2d/resource_types.h"

namespace gfx {
namespace impl {

class device_impl;

}  // namespace impl


struct image : base::non_copy_movable {
  virtual ~image() {}
  virtual impl::d2d_image_ptr d2d_image(impl::device_impl* device) const = 0;
};

}  // namespace gfx