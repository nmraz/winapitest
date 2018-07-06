#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/resource/cached_resource.h"

namespace gfx::impl {

template<typename T>
class cached_d2d_resource : public cached_resource {
public:
  using resource_ptr = base::win::com_ptr<T>;

  cached_d2d_resource(resource_ptr res)
    : resource_(std::move(res)) {}

  resource_ptr resource() const { return resource_; }

private:
  const resource_ptr resource_;
};

}  // namespace gfx::impl