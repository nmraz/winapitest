#pragma once

#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/image/device_image.h"

namespace gfx {

class device_bitmap : public device_image {
public:
  const bitmap_info& info() const { return info_; }
  const impl::d2d_bitmap_ptr& d2d_bitmap() const { return d2d_bitmap_; }

  impl::d2d_image_ptr d2d_image(impl::device_impl*) override { return d2d_bitmap(); }

protected:
  device_bitmap(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info);

private:
  impl::d2d_bitmap_ptr d2d_bitmap_;
  bitmap_info info_;
};

}  // namespace gfx