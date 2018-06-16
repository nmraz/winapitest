#include "mapped_texture.h"

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

}  // namespace gfx