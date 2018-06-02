#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/resource/cached_resource.h"

namespace gfx::impl {

template<typename T>
struct cached_d2d_resource : cached_resource {
  base::win::com_ptr<T> resource;
};

}  // namespace gfx::impl