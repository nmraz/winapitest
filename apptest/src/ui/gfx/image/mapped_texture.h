#pragma once

#include "base/non_copyable.h"
#include "base/span.h"
#include "ui/gfx/color.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"
#include <cstddef>

namespace gfx {

class mapped_texture : public base::non_copyable {
  ~mapped_texture();

  const bitmap_info& info() const { return info_; }

  sizef size() const;
  sizei pixel_size() const;
  int pitch() const { return mapped_.pitch; }

  base::span<const std::byte> pixels() const;
  color pixel_at(const pointi& pt) const;

private:
  friend class texture;

  mapped_texture(impl::d2d_bitmap_ptr mappable_bitmap, const bitmap_info& info);

  impl::d2d_bitmap_ptr bitmap_;
  D2D1_MAPPED_RECT mapped_;
  bitmap_info info_;
};

}  // namespace gfx