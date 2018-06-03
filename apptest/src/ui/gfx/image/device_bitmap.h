#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/image/device_image.h"
#include <d2d1_1.h>

namespace gfx {
namespace impl {

using d2d_bitmap_ptr = base::win::com_ptr<ID2D1Bitmap1>;

}  // namespace impl


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