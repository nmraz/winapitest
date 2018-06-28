#pragma once

#include "base/assert.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/matrix.h"
#include <optional>

namespace gfx::transform {

// For consistency with Direct2D (and contrary to mathematical convention),
// points are considered row vectors and are postmultiplied by the transforms themeselves.
// This also means that composition is "backwards" - `t * s` means first apply `t`, followed
// by `s`.

constexpr inline mat33f identity() {
  return {
    1, 0, 0,
    0, 1, 0,
    0, 0, 1
  };
}

constexpr inline mat33f translate(float dx, float dy) {
  return {
    1,  0,  0,
    0,  1,  0,
    dx, dy, 1
  };
}

constexpr inline mat33f scale(float sx, float sy) {
  return {
    sx, 0,  0,
    0,  sy, 0,
    0,  0,  1
  };
}

constexpr inline mat33f shear(float shx, float shy) {
  return {
    1,   shy, 0,
    shx, 1,   0,
    0,   0,   1
  };
}

mat33f rotate(float theta);

constexpr inline mat33f centered_about(const mat33f& tform, const pointf& center) {
  // translate `center` to origin, apply transform, and translate back
  return translate(-center.x(), -center.y()) * tform * translate(center.x(), center.y());
}


constexpr inline bool is_affine(const mat33f& tform) {
  return tform(0, 2) == 0 && tform(1, 2) == 0 && tform(2, 2) == 1;
}

constexpr inline bool is_scale_translate(const mat33f& tform) {
  return tform(0, 1) == 0 && tform(0, 2) == 0 && tform(1, 0) == 0
    && tform(1, 2) == 0 && tform(2, 2) == 1;
}


constexpr float determinant(const mat33f& tform) {
  ASSERT(is_affine(tform)) << "This only works for affine transforms";
  return tform(0, 0) * tform(1, 1) - tform(0, 1) * tform(1, 0);
}

constexpr inline bool is_invertible(const mat33f& tform) {
  return determinant(tform) != 0.f;
}

constexpr std::optional<mat33f> try_invert(const mat33f& tform) {
  float det = determinant(tform);
  if (!det) {
    return std::nullopt;
  }

  double inv_det = 1.0 / det;
  return mat33f{
    static_cast<float>(tform(1, 1) * inv_det),
    static_cast<float>(-tform(0, 1) * inv_det),
    0.f,
    static_cast<float>(-tform(1, 0) * inv_det),
    static_cast<float>(tform(0, 0) * inv_det),
    0.f,
    static_cast<float>((tform(1, 0) * tform(2, 1) - tform(1, 1) * tform(2, 0)) * inv_det),
    static_cast<float>((tform(0, 1) * tform(2, 0) - tform(0, 0) * tform(2, 1)) * inv_det),
    1.f
  };
}

constexpr mat33f invert(const mat33f& tform) {
  auto inverted = try_invert(tform);
  ASSERT(inverted) << "Transform not invertible";
  return *inverted;
}


constexpr pointf apply(const mat33f& tform, const pointf& pt) {
  ASSERT(is_affine(tform)) << "This only works for affine transforms";

  // postmultiply `pt` by `tform`
  return {
    pt.x() * tform(0, 0) + pt.y() * tform(1, 0) + tform(2, 0),
    pt.x() * tform(0, 1) + pt.y() * tform(1, 1) + tform(2, 1)
  };
}

}  // namespace gfx::transform