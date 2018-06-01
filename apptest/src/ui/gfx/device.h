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

class device : public base::non_copy_movable {
public:
  using ptr = std::shared_ptr<device>;

  static ptr create();
  static ptr create(impl::d3d_device_ptr d3d_device);

  bool is_lost() const;

  const impl::d3d_device_ptr& d3d_device() { return d3d_device_; }
  const impl::d2d_device_ptr& d2d_device() { return d2d_device_; }

private:
  device(impl::d3d_device_ptr d3d_device);

  impl::d3d_device_ptr d3d_device_;
  impl::d2d_device_ptr d2d_device_;
};

}  // namespace gfx