#include "mapped_texture.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/convs.h"

namespace gfx {

mapped_texture::~mapped_texture() {
  bitmap_->Unmap();
}


sizef mapped_texture::size() const {
  return impl::d2d_size_to_size(bitmap_->GetSize());
}

sizei mapped_texture::pixel_size() const {
  return impl::d2d_size_to_size(bitmap_->GetPixelSize());
}


base::span<const std::byte> mapped_texture::pixels() const {
  return { reinterpret_cast<const std::byte*>(mapped_.bits), pixel_size().height() * pitch() };
}


mapped_texture::mapped_texture(impl::d2d_bitmap_ptr mappable_bitmap, const bitmap_info& info)
  : bitmap_(std::move(mappable_bitmap))
  , info_(info) {
  base::win::throw_if_failed(bitmap_->Map(D2D1_MAP_OPTIONS_READ, &mapped_),
    "Failed to map GPU texture into memory");
}

}  // namespace gfx