#include "device_impl.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/d2d/factories.h"
#include "ui/gfx/image/bitmap_util.h"
#include <thread>

namespace gfx::impl {

void leased_dc_deleter::operator()(d2d_dc_ptr::element_type* dc) {
  dev_->return_dc(d2d_dc_ptr(dc));
}


device_impl::device_impl(impl::d3d_device_ptr d3d_device)
  : d3d_device_(std::move(d3d_device)) {
  auto dxgi_device = d3d_device_.as<IDXGIDevice>();
  base::win::throw_if_failed(
    get_d2d_factory()->CreateDevice(dxgi_device.get(), d2d_device_.addr()),
    "Failed to create Direct2D device"
  );
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

leased_dc device_impl::lease_dc() {
  {
    std::lock_guard hold(dc_pool_lock_);
    if (!dc_pool_.empty()) {
      auto dc = std::move(dc_pool_.back());
      dc_pool_.pop_back();
      return leased_dc(dc.detatch(), { this });
    }
  }
  return leased_dc(create_dc().detatch(), { this });
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
  int pitch = compute_pitch(static_cast<int>(data.size()), size.height(), info.format());

  d2d_bitmap_ptr bitmap;
  base::win::throw_if_failed(
    lease_dc()->CreateBitmap(
      size_to_d2d_size(size),
      data.data(),
      pitch,
      props,
      bitmap.addr()
    ),
    "Failed to create bitmap"
  );
  return bitmap;
}


// PRIVATE

void device_impl::return_dc(d2d_dc_ptr dc) {
  static const auto max_pool_size = std::max(std::thread::hardware_concurrency(), 1u);

  std::lock_guard hold(dc_pool_lock_);
  if (dc_pool_.size() < max_pool_size) {
    dc_pool_.push_back(std::move(dc));
  }
}

}  // namespace gfx::impl