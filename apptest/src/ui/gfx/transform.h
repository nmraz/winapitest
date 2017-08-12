#pragma once

#include "ui/gfx/geom/matrix.h"
#include "ui/gfx/geom/point.h"

namespace gfx::transform {

// For consistency with Direct2D, points are considered row vectors
// and are postmultiplied by the transforms themeselves. This also
// provides a more intuitive syntax, where `A * B` means "first apply
// A, followed by B".

constexpr mat33f identity() {
	return {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
}

mat33f translate(float dx, float dy);
mat33f scale(float sx, float sy);
mat33f shear(float shx, float shy);
mat33f rotate(float theta);

mat33f centered_about(const mat33f& tform, const pointf& center);


pointf apply(const mat33f& tform, const pointf& pt);

}  // namespace gfx::transform