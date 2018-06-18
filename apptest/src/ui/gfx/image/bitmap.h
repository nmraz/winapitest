#pragma once

#include "base/span.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include "ui/gfx/image/image.h"
#include <cstddef>
#include <memory>
#include <vector>

namespace gfx {

class bitmap : public image {
public:
  std::unique_ptr<bitmap> create(const bitmap_info& info, const sizei& size,
    base::span<const std::byte> data);

  const bitmap_info& info() const { return info_; }

  sizef size() const;
  sizei pixel_size() const { return pixel_size_; }
  int pitch() const;

  base::span<const std::byte> pixels() const { return pixels_; }

private:
  std::vector<std::byte> pixels_;
  sizei pixel_size_;
  bitmap_info info_;
};

}  // namespace gfx