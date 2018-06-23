#pragma once

#include "base/span.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/image/bitmap_lock.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/resource/resource_key.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace gfx {

class bitmap : public image {
public:
  static std::unique_ptr<bitmap> create(const bitmap_info& info, const sizei& size);
  static std::unique_ptr<bitmap> create(const bitmap_info& info, const sizei& size,
    base::span<const std::byte> data);
  
  template<typename Bmp>
  static std::unique_ptr<bitmap> create(const Bmp& bmp) {
    return create(bmp.info(), bmp.pixel_size(), bmp.pixels());
  }


  const bitmap_info& info() const { return info_; }

  sizef size() const;
  sizei pixel_size() const { return pixel_size_; }
  int pitch() const;

  base::span<const std::byte> pixels() const { return pixels_; }
  
  bitmap_lock lock();
  bool is_locked() const { return lock_count_ > 0; }

  impl::d2d_image_ptr d2d_image(impl::device_impl* dev) const override;

private:
  friend class bitmap_lock;

  bitmap(const bitmap_info& info, const sizei& size, std::vector<std::byte> pixels);
  void unlock();

  std::vector<std::byte> pixels_;
  sizei pixel_size_;
  bitmap_info info_;

  resource_key key_;
  int lock_count_ = 0;
};

}  // namespace gfx