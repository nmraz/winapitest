#pragma once

#include "base/non_copyable.h"
#include "base/span.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/image/bitmap_info.h"

namespace gfx {

class bitmap;

class bitmap_lock : public base::non_copyable {
public:
  ~bitmap_lock();

  const bitmap_info& info() const;

  sizef size() const;
  sizei pixel_size() const;
  int pitch() const;

  base::span<std::byte> pixels();
  base::span<const std::byte> pixels() const;

private:
  friend class bitmap;

  bitmap_lock(bitmap& bmp);

  bitmap& bmp_;
};

}  // namespace gfx