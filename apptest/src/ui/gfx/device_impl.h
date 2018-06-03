#pragma once

#include "base/span.h"
#include "ui/gfx/device.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/resource/resource_cache.h"
#include <cstddef>

namespace gfx::impl {

class device_impl : public device {
public:
  device_impl(impl::d3d_device_ptr d3d_device);

  bool is_lost() const override;

  const d3d_device_ptr& d3d_device() { return d3d_device_; }
  const d2d_device_ptr& d2d_device() { return d2d_device_; }

  d2d_dc_ptr create_dc();
  d2d_bitmap_ptr create_bitmap(const bitmap_info& info, const sizei& size, 
    D2D1_BITMAP_OPTIONS opts = D2D1_BITMAP_OPTIONS_NONE, base::span<const std::byte> data = {});

  const d2d_dc_ptr& resource_dc() { return resource_dc_; }
  resource_cache& cache() { return cache_; }

private:
  d3d_device_ptr d3d_device_;
  d2d_device_ptr d2d_device_;

  d2d_dc_ptr resource_dc_;

  resource_cache cache_;
};

}  // namespace gfx::impl