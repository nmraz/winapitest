#pragma once

#include "base/non_copyable.h"
#include "base/win/com_ptr.h"
#include "ui/gfx/device_impl.h"
#include <d2d1_1.h>

namespace gfx {
namespace impl {

using d2d_image_ptr = base::win::com_ptr<ID2D1Image>;

}  // namespace impl


struct image : base::non_copy_movable {
  virtual ~image() {}
  virtual impl::d2d_image_ptr d2d_image(impl::device_impl* device) = 0;
};

}  // namespace gfx