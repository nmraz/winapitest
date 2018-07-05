#pragma once

#include "ui/gfx/brush/brush.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/resource/resource_key.h"
#include <d2d1_1.h>

namespace gfx {

class image_brush : public brush {
public:
  enum class extend_mode {
    clamp = D2D1_EXTEND_MODE_CLAMP,
    repeat = D2D1_EXTEND_MODE_WRAP,
    mirror = D2D1_EXTEND_MODE_MIRROR
  };

  static std::unique_ptr<image_brush> create();
  static std::unique_ptr<image_brush> create(const image& img, const rectf& src_rect);
  template<typename Img>
  static std::unique_ptr<image_brush> create(const Img& img) {
    return create(img, rectf({}, img.size()));
  }

  const image* img() const { return img_; }
  void set_img(const image* img) { img_ = img; }

  image::interpolation_mode interp_mode() const { return interp_mode_; }
  void set_interp_mode(image::interpolation_mode mode) {
    interp_mode_ = mode;
  }

  const rectf& src_rect() const { return src_rect_; }
  void set_src_rect(const rectf& rc) { src_rect_ = rc; }

  extend_mode extend_mode_x() const { return extend_mode_x_; }
  void set_extend_mode_x(extend_mode mode) {
    extend_mode_x_ = mode;
  }

  extend_mode extend_mode_y() const { return extend_mode_y_; }
  void set_extend_mode_y(extend_mode mode) {
    extend_mode_y_ = mode;
  }

private:
  image_brush() = default;

  impl::d2d_brush_ptr do_get_d2d_brush(impl::device_impl* dev) const override;

  const image* img_ = nullptr;
  image::interpolation_mode interp_mode_ = image::interpolation_mode::linear;
  rectf src_rect_;

  extend_mode extend_mode_x_ = extend_mode::clamp;
  extend_mode extend_mode_y_ = extend_mode::clamp;

  resource_key key_;
};

}  // namespace gfx