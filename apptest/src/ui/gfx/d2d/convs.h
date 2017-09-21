#pragma once

#include "ui/gfx/color.h"
#include "ui/gfx/geom/matrix.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/geom/size.h"
#include <d2d1_1.h>


namespace gfx::impl {

constexpr D2D1_COLOR_F color_to_d2d_color(color col) {
	return {
		col.r / 255.f,
		col.g / 255.f,
		col.b / 255.f,
		col.a / 255.f
	};
}


constexpr D2D1_POINT_2F point_to_d2d_point(const pointf& pt) {
	return { pt.x, pt.y };
}

constexpr pointf d2d_point_to_point(const D2D1_POINT_2F& pt) {
	return { pt.x, pt.y };
}


constexpr D2D1_SIZE_F size_to_d2d_size(const sizef& sz) {
	return { sz.width, sz.height };
}

constexpr sizef d2d_size_to_size(const D2D1_SIZE_F& sz) {
	return { sz.width, sz.height };
}


constexpr D2D1_RECT_F rect_to_d2d_rect(const rectf& rc) {
	return {
		rc.x(),
		rc.y(),
		rc.bottom(),
		rc.right()
	};
}

constexpr rectf d2d_rect_to_rect(const D2D1_RECT_F& rc) {
	return {
		rectf::by_bounds,
		rc.top,
		rc.left,
		rc.bottom,
		rc.right
	};
}


constexpr D2D1_MATRIX_3X2_F mat33_to_d2d_mat32(const mat33f& mat) {
	return {
		mat.get(0, 0), mat.get(0, 1),
		mat.get(1, 0), mat.get(1, 1),
		mat.get(2, 0), mat.get(2, 1)
	};
}

constexpr mat33f d2d_mat32_to_mat33(const D2D1_MATRIX_3X2_F& mat) {
	return {
		mat._11, mat._12, 0,
		mat._21, mat._22, 0,
		mat._31, mat._32, 1
	};
}

}  // namespace gfx::impl