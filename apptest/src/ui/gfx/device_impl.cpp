#include "device_impl.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/factories.h"

namespace gfx::impl {

bool device_impl::is_lost() const {
  return FAILED(d3d_device_->GetDeviceRemovedReason());
}

device_impl::device_impl(impl::d3d_device_ptr d3d_device)
  : d3d_device_(std::move(d3d_device)) {
  auto dxgi_device = d3d_device_.as<IDXGIDevice>();
  base::win::throw_if_failed(
    get_d2d_factory()->CreateDevice(dxgi_device.get(), d2d_device_.addr()),
    "Failed to create Direct2D device"
  );
}

}  // namespace gfx::impl