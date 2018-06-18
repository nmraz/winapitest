#include "mapped_texture.h"

#include "base/assert.h"
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

color mapped_texture::pixel_at(const pointi& pt) const {
  ASSERT(pt.x() < pixel_size().width() && pt.y() < pixel_size().height()) << "Point out of range";

  int offset = pixel_offset(pt.x(), pt.y(), pitch(), info().format());
  return read_pixel(pixels().data() + offset, info().format(), info().alpha());
}


mapped_texture::mapped_texture(impl::d2d_bitmap_ptr mappable_bitmap, const bitmap_info& info)
  : bitmap_(std::move(mappable_bitmap))
  , info_(info) {
  base::win::throw_if_failed(bitmap_->Map(D2D1_MAP_OPTIONS_READ, &mapped_),
    "Failed to map GPU texture into memory");
}

}  // namespace gfx