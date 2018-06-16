#include "texture.h"

#include "ui/gfx/device_impl.h"
#include "ui/gfx/d2d/convs.h"

namespace gfx {
namespace {

bitmap_info info_from_d2d_bitmap(const ID2D1Bitmap1* bitmap) {
  auto pix_format = bitmap->GetPixelFormat();
  float dpix, dpiy;
  bitmap->GetDpi(&dpix, &dpiy);

  return {
    static_cast<pixel_format>(pix_format.format),
    static_cast<alpha_mode>(pix_format.alphaMode),
    dpix,
    dpiy
  };
}

}  // namespace


std::unique_ptr<texture> texture::create(device::ptr dev, const bitmap_info& info,
  const sizei& size, base::span<const std::byte> data) {
  auto* dev_impl = static_cast<impl::device_impl*>(dev.get());

  return std::unique_ptr<texture>(new texture(std::move(dev),
    dev_impl->create_bitmap(info, size, D2D1_BITMAP_OPTIONS_NONE, data), info));
}

std::unique_ptr<texture> texture::create(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap) {
  return std::unique_ptr<texture>(new texture(std::move(dev), std::move(d2d_bitmap),
    info_from_d2d_bitmap(d2d_bitmap.get())));
}


sizef texture::size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetSize());
}

sizei texture::pixel_size() const {
  return impl::d2d_size_to_size(d2d_bitmap_->GetPixelSize());
}


mapped_texture texture::map() const {
  impl::device_impl* dev = static_cast<impl::device_impl*>(device());
  impl::d2d_bitmap_ptr mappable_bitmap = dev->create_bitmap(info(), pixel_size(),
    D2D1_BITMAP_OPTIONS_CPU_READ);

  D2D1_POINT_2U dest_pt = { 0, 0 };
  D2D1_RECT_U src_rect = { 0, 0, pixel_size().width(), pixel_size().height() };

  mappable_bitmap->CopyFromBitmap(&dest_pt, d2d_bitmap_.get(), &src_rect);
  return { std::move(mappable_bitmap), info() };
}


texture::texture(device::ptr dev, impl::d2d_bitmap_ptr d2d_bitmap, const bitmap_info& info)
  : device_image(std::move(dev))
  , d2d_bitmap_(std::move(d2d_bitmap))
  , info_(info) {
}

}  // namespac gfx