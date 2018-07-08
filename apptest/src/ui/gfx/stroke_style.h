#pragma once

#include "base/span.h"
#include "base/win/com_ptr.h"
#include <d2d1_1.h>
#include <mutex>
#include <vector>

namespace gfx {
namespace impl {

using d2d_stroke_style_ptr = base::win::com_ptr<ID2D1StrokeStyle1>;

}  // namespace impl


enum class stroke_cap_style {
  flat = D2D1_CAP_STYLE_FLAT,
  square = D2D1_CAP_STYLE_SQUARE,
  round = D2D1_CAP_STYLE_ROUND,
  tri = D2D1_CAP_STYLE_TRIANGLE
};

enum class stroke_line_join {
  miter = D2D1_LINE_JOIN_MITER,
  bevel = D2D1_LINE_JOIN_BEVEL,
  round = D2D1_LINE_JOIN_ROUND,
  limited_miter = D2D1_LINE_JOIN_MITER_OR_BEVEL
};

enum class stroke_transform_type {
  normal = D2D1_STROKE_TRANSFORM_TYPE_NORMAL,
  fixed = D2D1_STROKE_TRANSFORM_TYPE_FIXED,
  hairline = D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE
};


class stroke_style {
public:
  stroke_transform_type transform_type() const { return transform_type_; }
  void set_transform_type(stroke_transform_type tform);


  stroke_cap_style start_cap() const { return start_cap_; }
  void set_start_cap(stroke_cap_style cap);

  stroke_cap_style end_cap() const { return end_cap_; }
  void set_end_cap(stroke_cap_style cap);


  stroke_line_join line_join() const { return line_join_; }
  void set_line_join(stroke_line_join join);

  float miter_limit() const { return miter_limit_; }
  void set_miter_limit(float limit);


  base::span<const float> dash_pattern() const;
  
  template<typename T>
  void set_dash_pattern(const T& pattern) {
    using std::begin;  // ADL

    mark_dirty();
    dash_pattern_.assign(begin(pattern), end(pattern));
  }
  void set_dash_pattern(std::initializer_list<float> pattern) {
    mark_dirty();
    dash_pattern_ = pattern;
  }

  stroke_cap_style dash_cap() const { return dash_cap_; }
  void set_dash_cap(stroke_cap_style cap);

  float dash_offset() const { return dash_offset_; }
  void set_dash_offset(float offset);
  

  const impl::d2d_stroke_style_ptr& d2d_stroke_style() const;
  void make_thread_safe() const;

private:
  void mark_dirty();

  stroke_transform_type transform_type_ = stroke_transform_type::normal;

  stroke_cap_style start_cap_ = stroke_cap_style::flat;
  stroke_cap_style end_cap_ = stroke_cap_style::flat;

  stroke_line_join line_join_ = stroke_line_join::miter;
  float miter_limit_ = 10;

  std::vector<float> dash_pattern_;
  stroke_cap_style dash_cap_ = stroke_cap_style::flat;
  float dash_offset_ = 0;

  mutable impl::d2d_stroke_style_ptr d2d_stroke_style_;
};

}  // namespace gfx