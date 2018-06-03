#include "device_impl.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/d2d/factories.h"

namespace gfx::impl {

device_impl::device_impl(impl::d3d_device_ptr d3d_device)
  : d3d_device_(std::move(d3d_device)) {
  auto dxgi_device = d3d_device_.as<IDXGIDevice>();
  base::win::throw_if_failed(
    get_d2d_factory()->CreateDevice(dxgi_device.get(), d2d_device_.addr()),
    "Failed to create Direct2D device"
  );
  resource_dc_ = create_dc();
}

bool device_impl::is_lost() const {
  return FAILED(d3d_device_->GetDeviceRemovedReason());
}


d2d_dc_ptr device_impl::create_dc() {
  d2d_dc_ptr dc;
  base::win::throw_if_failed(
    d2d_device()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, dc.addr()),
    "Failed to create Direct2D device context"
  );
  return dc;
}

d2d_bitmap_ptr device_impl::create_bitmap(const bitmap_info& info, const sizei& size,
  D2D1_BITMAP_OPTIONS opts, base::span<const std::byte> data) {
  auto props = D2D1::BitmapProperties1(
    opts,
    D2D1::PixelFormat(
      static_cast<DXGI_FORMAT>(info.format()),
      static_cast<D2D1_ALPHA_MODE>(info.alpha())
    ),
    info.dpix(),
    info.dpiy()
  );

  d2d_bitmap_ptr bitmap;
  base::win::throw_if_failed(
    resource_dc()->CreateBitmap(
      size_to_d2d_size(size),
      data.data(),
      static_cast<UINT32>(data.size() / size.height()),
      props,
      bitmap.addr()
    ),
    "Failed to create bitmap"
  );
  return bitmap;
}

}  // namespace gfx::impl