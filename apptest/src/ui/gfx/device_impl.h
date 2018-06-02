#pragma once

#include "ui/gfx/device.h"
#include "ui/gfx/resource/resource_cache.h"

namespace gfx::impl {

class device_impl : public device {
public:
  device_impl(impl::d3d_device_ptr d3d_device);

  bool is_lost() const override;

  const d3d_device_ptr& d3d_device() { return d3d_device_; }
  const d2d_device_ptr& d2d_device() { return d2d_device_; }

  d2d_dc_ptr create_dc();
  const d2d_dc_ptr& resource_dc() { return resource_dc_; }

  resource_cache& cache() { return cache_; }

private:
  d3d_device_ptr d3d_device_;
  d2d_device_ptr d2d_device_;

  d2d_dc_ptr resource_dc_;

  resource_cache cache_;
};

}  // namespace gfx::impl