#include "stroke_style.h"

#include "base/win/last_error.h"
#include "ui/gfx/d2d/factories.h"

namespace gfx {
namespace {

auto create_d2d_stroke_style(const D2D1_STROKE_STYLE_PROPERTIES1& props, base::span<const float> dashes) {
  impl::d2d_stroke_style_ptr stroke_style;
  base::win::throw_if_failed(
    impl::get_d2d_factory()->CreateStrokeStyle(
      props,
      dashes.data(),
      static_cast<UINT>(dashes.size()),
      stroke_style.addr()
    ),
    "Failed to create stroke style"
  );
  return stroke_style;
}

}  // namespace


void stroke_style::set_transform_type(stroke_transform_type tform) {
  mark_dirty();
  transform_type_ = tform;
}


void stroke_style::set_start_cap(stroke_cap_style cap) {
  mark_dirty();
  start_cap_ = cap;
}

void stroke_style::set_end_cap(stroke_cap_style cap) {
  mark_dirty();
  end_cap_ = cap;
}


void stroke_style::set_line_join(stroke_line_join join) {
  mark_dirty();
  line_join_ = join;
}

void stroke_style::set_miter_limit(float limit) {
  mark_dirty();
  miter_limit_ = limit;
}


base::span<const float> stroke_style::dash_pattern() const {
  return dash_pattern_;
}

void stroke_style::set_dash_cap(stroke_cap_style cap) {
  mark_dirty();
  dash_cap_ = cap;
}

void stroke_style::set_dash_offset(float offset) {
  mark_dirty();
  dash_offset_ = offset;
}


const impl::d2d_stroke_style_ptr& stroke_style::d2d_stroke_style() const {
  std::lock_guard hold(d2d_stroke_style_lock_);

  if (!d2d_stroke_style_) {
    D2D1_DASH_STYLE dash_style = dash_pattern().empty() ?
      D2D1_DASH_STYLE_SOLID : D2D1_DASH_STYLE_CUSTOM;

    d2d_stroke_style_ = create_d2d_stroke_style(
      D2D1::StrokeStyleProperties1(
        static_cast<D2D1_CAP_STYLE>(start_cap()),
        static_cast<D2D1_CAP_STYLE>(end_cap()),
        static_cast<D2D1_CAP_STYLE>(dash_cap()),
        static_cast<D2D1_LINE_JOIN>(line_join()),
        miter_limit(),
        dash_style,
        dash_offset(),
        static_cast<D2D1_STROKE_TRANSFORM_TYPE>(transform_type())
      ),
      dash_pattern()
    );
  }
  return d2d_stroke_style_;  // guaranteed not to change until a non-const operation is performed
}


// PRIVATE

void stroke_style::mark_dirty() {
  d2d_stroke_style_ = nullptr;  // no lock - safety is only guaranteed on const operations
}

}  // namespace gfx