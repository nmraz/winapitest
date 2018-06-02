#pragma once

#include "base/non_copyable.h"

namespace gfx {
namespace impl {

struct image_impl;

}  // namespace impl


struct image : base::non_copy_movable {
  virtual ~image() {}
  virtual const impl::image_impl* impl() const = 0;
};

}  // namespace gfx