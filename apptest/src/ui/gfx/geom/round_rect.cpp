#include "round_rect.h"

#include <algorithm>

namespace gfx {
namespace {

void get_scale(float r1, float r2, float max_size, float& scale) {
  if (r1 + r2 > max_size) {
    scale = std::min(scale, max_size / (r1 + r2));
  }
}

}  // namespace

round_rect::round_rect(const rectf& bounds, const sizef& radius)
  : round_rect(bounds, radius, radius, radius, radius) {
}

round_rect::round_rect(const rectf& bounds, const sizef& top_left, const sizef& top_right,
  const sizef& bottom_right, const sizef& bottom_left)
  : bounds_(bounds)
  , radii_{ top_left, top_right, bottom_right, bottom_left } {
  scale_radii();
}


const sizef& round_rect::radius(rect_corner corner) const {
  return radii_[static_cast<int>(corner)];
}

void round_rect::set_radius(rect_corner corner, const sizef& radius) {
  radii_[static_cast<int>(corner)] = radius;
  scale_radii();
}


bool round_rect::contains(const pointf& pt) {
  if (!bounds_.contains(pt)) {
    return false;
  }

  rect_corner containing_corner;
  pointf local_pt;

  if (pt.x < bounds_.x() + radius(rect_corner::top_left).width()
    && pt.y < bounds_.y() + radius(rect_corner::top_left).height()) {  // top-left

    containing_corner = rect_corner::top_left;
    local_pt.set(
      bounds_.x() + radius(rect_corner::top_left).width() - pt.x,
      bounds_.y() + radius(rect_corner::top_left).height() - pt.y
    );

  } else if (pt.x > bounds_.right() - radius(rect_corner::top_right).width()
    && pt.y < bounds_.y() + radius(rect_corner::top_right).height()) {  // top-right

    containing_corner = rect_corner::top_right;
    local_pt.set(
      pt.x - bounds_.right() + radius(rect_corner::top_right).width(),
      bounds_.y() + radius(rect_corner::top_right).height() - pt.y
    );

  } else if (pt.x > bounds_.right() - radius(rect_corner::bottom_right).width()
    && pt.y > bounds_.bottom() - radius(rect_corner::bottom_right).height()) {  // bottom-right

    containing_corner = rect_corner::bottom_right;
    local_pt.set(
      pt.x - bounds_.right() + radius(rect_corner::bottom_right).width(),
      pt.y - bounds_.bottom() + radius(rect_corner::bottom_right).height()
    );

  } else if (pt.x < bounds_.x() + radius(rect_corner::bottom_left).width()
    && pt.y > bounds_.bottom() - radius(rect_corner::bottom_left).height()) {  // bottom-left

    containing_corner = rect_corner::bottom_left;
    local_pt.set(
      bounds_.x() + radius(rect_corner::bottom_left).width() - pt.y,
      pt.y - bounds_.bottom() + radius(rect_corner::bottom_left).height()
    );

  } else {  // not in a corner
    return true;
  }

  float rx = radius(containing_corner).width();
  float ry = radius(containing_corner).height();

  // ellipse equation, multiplied by (rx*ry)^2
  return ry * ry * local_pt.x * local_pt.x + rx * rx * local_pt.y * local_pt.y <= rx * rx * ry * ry;
}


// PRIVATE

void round_rect::scale_radii() {
  float scale = 1.f;

  get_scale(
    radius(rect_corner::top_left).width(),
    radius(rect_corner::top_right).width(),
    bounds().width(),
    scale
  );
  get_scale(
    radius(rect_corner::top_right).height(),
    radius(rect_corner::bottom_right).height(),
    bounds().height(),
    scale
  );
  get_scale(
    radius(rect_corner::bottom_right).width(),
    radius(rect_corner::bottom_left).width(),
    bounds().width(),
    scale
  );
  get_scale(
    radius(rect_corner::bottom_left).height(),
    radius(rect_corner::top_left).height(),
    bounds().height(),
    scale
  );

  if (scale < 1.f) {
    for (sizef& rad : radii_) {
      rad *= scale;
    }
  }
}

}  // namespace gfx