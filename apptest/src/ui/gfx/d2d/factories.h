#pragma once

#include "base/win/com_ptr.h"
#include <d2d1_1.h>
#include <dwrite.h>

namespace gfx::impl {

const base::win::com_ptr<ID2D1Factory1>& get_d2d_factory();
const base::win::com_ptr<IDWriteFactory>& get_dwrite_factory();

}  // namespace gfx::impl