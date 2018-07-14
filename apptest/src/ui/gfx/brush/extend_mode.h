#pragma once

#include <d2d1_1.h>

namespace gfx {

enum class extend_mode {
  clamp = D2D1_EXTEND_MODE_CLAMP,
  repeat = D2D1_EXTEND_MODE_WRAP,
  mirror = D2D1_EXTEND_MODE_MIRROR
};

}  // namespace gfx