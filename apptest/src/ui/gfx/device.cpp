#include "device.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/factories.h"

namespace gfx {
namespace {

HRESULT create_d3d_device(D3D_DRIVER_TYPE type, impl::d3d_device_ptr& device) {
  UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifndef NDEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  return ::D3D11CreateDevice(
    nullptr,
    type,
    nullptr,
    flags,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    device.addr(),
    nullptr,
    nullptr
  );
}

}  // namespace


device::ptr device::create() {
  impl::d3d_device_ptr d3d_device;

  if (FAILED(create_d3d_device(D3D_DRIVER_TYPE_HARDWARE, d3d_device))) {
    base::win::throw_if_failed(
      create_d3d_device(D3D_DRIVER_TYPE_WARP, d3d_device),
      "Failed to create Direct3D device"
    );
  }

  return create(std::move(d3d_device));
}

device::ptr device::create(impl::d3d_device_ptr d3d_device) {
  return ptr(new device(std::move(d3d_device)));
}


bool device::is_lost() const {
  return FAILED(d3d_device_->GetDeviceRemovedReason());
}


// PRIVATE

device::device(impl::d3d_device_ptr d3d_device)
  : d3d_device_(std::move(d3d_device)) {
  auto dxgi_device = d3d_device_.as<IDXGIDevice>();
  base::win::throw_if_failed(
    impl::get_d2d_factory()->CreateDevice(dxgi_device.get(), d2d_device_.addr()),
    "Failed to create Direct2D device"
  );
}

}  // namespace gfx