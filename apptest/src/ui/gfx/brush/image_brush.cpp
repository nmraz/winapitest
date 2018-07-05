#include "image_brush.h"

#include "ui/gfx/d2d/cached_d2d_resource.h"
#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/device_impl.h"

namespace gfx {
namespace {

impl::d2d_image_ptr maybe_get_d2d_image(const image* img, impl::device_impl* dev) {
  return img ? img->d2d_image(dev) : nullptr;
}

}  // namespace


std::unique_ptr<image_brush> image_brush::create() {
  return std::unique_ptr<image_brush>(new image_brush());
}

std::unique_ptr<image_brush> image_brush::create(const image& img, const rectf& src_rect) {
  auto brush = create();
  brush->set_img(&img);
  brush->set_src_rect(src_rect);
  return brush;
}

std::unique_ptr<image_brush> image_brush::create(const image& img) {
  return create(img, img.bounds());
}


// PRIVATE

impl::d2d_brush_ptr image_brush::do_get_d2d_brush(impl::device_impl* dev) const {
  auto brush = dev->cache().find_or_create(&key_, [&] {
    base::win::com_ptr<ID2D1ImageBrush> brush;
    dev->lease_dc()->CreateImageBrush(nullptr, D2D1::ImageBrushProperties({}), brush.addr());
    
    return std::make_unique<impl::cached_d2d_resource<ID2D1ImageBrush>>(
      key_.version(),
      std::move(brush)
    );
  })->resource();

  brush->SetImage(maybe_get_d2d_image(img(), dev).get());
  brush->SetInterpolationMode(static_cast<D2D1_INTERPOLATION_MODE>(interp_mode()));

  auto d2d_src_rect = impl::rect_to_d2d_rect(src_rect());
  brush->SetSourceRectangle(&d2d_src_rect);

  brush->SetExtendModeX(static_cast<D2D1_EXTEND_MODE>(extend_mode_x()));
  brush->SetExtendModeY(static_cast<D2D1_EXTEND_MODE>(extend_mode_y()));

  return brush;
}

}  // namespace gfx