#pragma once

#include "base/win/com_ptr.h"
#include <d2d1_1.h>
#include <d3d11.h>

namespace gfx::impl {

using d3d_device_ptr = base::win::com_ptr<ID3D11Device>;
using d2d_device_ptr = base::win::com_ptr<ID2D1Device>;
using d2d_dc_ptr = base::win::com_ptr<ID2D1DeviceContext>;

using d2d_image_ptr = base::win::com_ptr<ID2D1Image>;
using d2d_bitmap_ptr = base::win::com_ptr<ID2D1Bitmap1>;

using d2d_brush_ptr = base::win::com_ptr<ID2D1Brush>;

}  // namespace gfx::impl