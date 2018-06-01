#pragma once

#include "base/win/com_ptr.h"
#include <d2d1_1.h>
#include <d3d11.h>
#include <memory>

namespace gfx {

class device {
public:
  using ptr = std::shared_ptr<device>;

  static ptr create();
  static ptr create(base::win::com_ptr<ID3D11Device> d3d_device);

  bool is_lost() const;

  const base::win::com_ptr<ID3D11Device>& d3d_device();
  const base::win::com_ptr<ID2D1Device>& d2d_device();

private:
  device(base::win::com_ptr<ID3D11Device> d3d_device);

  base::win::com_ptr<ID3D11Device> d3d_device_;
  base::win::com_ptr<ID2D1Device> d2d_device_;
};

}  // namespace gfx