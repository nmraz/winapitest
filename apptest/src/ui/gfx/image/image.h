#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/d2d/resource_types.h"
#include "ui/gfx/geom/rect.h"

namespace gfx {
namespace impl {

class device_impl;

}  // namespace impl


struct image : base::non_copy_movable {
  enum class interpolation_mode {
    nearest_neighbor = D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
    linear = D2D1_INTERPOLATION_MODE_DEFINITION_LINEAR,
    cubic = D2D1_INTERPOLATION_MODE_CUBIC,
    multi_sample_linear = D2D1_INTERPOLATION_MODE_MULTI_SAMPLE_LINEAR,
    anisotropic = D2D1_INTERPOLATION_MODE_ANISOTROPIC,
    high_quality_cubic = D2D1_INTERPOLATION_MODE_DEFINITION_HIGH_QUALITY_CUBIC
  };

  virtual ~image() {}
  virtual rectf bounds() const = 0;

  virtual impl::d2d_image_ptr d2d_image(impl::device_impl* device) const = 0;
};

}  // namespace gfx