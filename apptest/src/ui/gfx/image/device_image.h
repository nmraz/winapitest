#pragma once

#include "ui/gfx/device.h"
#include "ui/gfx/image/image.h"

namespace gfx {

class device_image : public image {
public:
  device* device() const { return device_.get(); }
  device::ptr ref_device() const { return device_; }

  bool is_valid() const { return !device()->is_lost(); }

protected:
  device_image(device::ptr dev) : device_(std::move(dev)) {}

private:
  device::ptr device_;
};

}  // namespace gfx