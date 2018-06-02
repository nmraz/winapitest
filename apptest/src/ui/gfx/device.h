#pragma once

#include "base/non_copyable.h"
#include "base/win/com_ptr.h"
#include <d2d1_1.h>
#include <d3d11.h>
#include <memory>

namespace gfx {
namespace impl {

using d3d_device_ptr = base::win::com_ptr<ID3D11Device>;
using d2d_device_ptr = base::win::com_ptr<ID2D1Device>;

}  // namespace impl


struct device : public base::non_copy_movable {
  using ptr = std::shared_ptr<device>;

  static ptr create();
  static ptr create(impl::d3d_device_ptr d3d_device);

  virtual ~device() {}
  virtual bool is_lost() const = 0;
};

}  // namespace gfx