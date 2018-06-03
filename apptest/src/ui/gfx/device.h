#pragma once

#include "base/non_copyable.h"
#include "ui/gfx/d2d/resource_types.h"
#include <memory>

namespace gfx {

struct device : public base::non_copy_movable {
  using ptr = std::shared_ptr<device>;

  static ptr create();
  static ptr create(impl::d3d_device_ptr d3d_device);

  virtual ~device() {}
  virtual bool is_lost() const = 0;
};

}  // namespace gfx