#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/matrix.h"

namespace gfx {

class brush : public base::non_copy_movable {
public:
  virtual ~brush() {}

  const mat33f& transform() const;
  void set_transform(const mat33f& tform);

  float opacity() const;
  void set_opacity(float opacity);
};

}  // namespace gfx