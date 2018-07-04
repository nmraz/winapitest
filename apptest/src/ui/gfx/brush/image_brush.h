#pragma once

#include "ui/gfx/brush/brush.h"
#include "ui/gfx/image/image.h"

namespace gfx {

class image_brush : public brush {
public:
  static std::unique_ptr<image_brush> create(const image* img = nullptr);

  const image* img() const;
  void set_img(const image* img);

private:
  const image* img_;
};

}  // namespace gfx