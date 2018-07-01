#pragma once

#include "base/span.h"
#include "ui/gfx/device.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/resource/resource_cache.h"
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

namespace gfx::impl {

class leased_dc_deleter {
public:
  void operator()(d2d_dc_ptr::element_type* dc);

private:
  friend class device_impl;

  leased_dc_deleter(device_impl* dev) : dev_(dev) {}
  device_impl* dev_;
};

using leased_dc = std::unique_ptr<d2d_dc_ptr::element_type, leased_dc_deleter>;


class device_impl : public device {
public:
  device_impl(impl::d3d_device_ptr d3d_device);

  bool is_lost() const override;

  const d3d_device_ptr& d3d_device() { return d3d_device_; }
  const d2d_device_ptr& d2d_device() { return d2d_device_; }

  d2d_dc_ptr create_dc();
  leased_dc lease_dc();
  
  d2d_bitmap_ptr create_bitmap(const bitmap_info& info, const sizei& size, 
    D2D1_BITMAP_OPTIONS opts = D2D1_BITMAP_OPTIONS_NONE, base::span<const std::byte> data = {});

  resource_cache& cache() { return cache_; }

private:
  friend class leased_dc_deleter;

  void return_dc(d2d_dc_ptr dc);

  d3d_device_ptr d3d_device_;
  d2d_device_ptr d2d_device_;

  std::vector<d2d_dc_ptr> dc_pool_;
  std::mutex dc_pool_lock_;

  resource_cache cache_;
};

}  // namespace gfx::impl