#pragma once

#include "base/span.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/image/device_image.h"
#include "ui/gfx/image/mapped_texture.h"
#include <memory>

namespace gfx {

class texture : public device_image {
public:
  static std::unique_ptr<texture> create(device::ptr dev, const bitmap_info& info,
    const sizei& size, base::span<const std::byte> data);
  static std::unique_ptr<texture> create(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap);

  template<typename Bmp>
  static std::unique_ptr<texture> create(device::ptr dev, const Bmp& bmp) {
    return create(std::move(dev), bmp.info(), bmp.pixel_size(), bmp.pixels());
  }


  const bitmap_info& info() const { return info_; }

  sizef size() const;
  sizei pixel_size() const;

  mapped_texture map() const;

  const impl::d2d_bitmap_ptr& d2d_bitmap() const { return d2d_bitmap_; }
  impl::d2d_image_ptr d2d_image(impl::device_impl*) const override { return d2d_bitmap(); }

protected:
  texture(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info);

private:
  impl::d2d_bitmap_ptr d2d_bitmap_;
  bitmap_info info_;
};

}  // namespace gfx