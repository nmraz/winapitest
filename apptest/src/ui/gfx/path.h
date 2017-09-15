#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/geom/size.h"
#include <d2d1_1.h>

namespace gfx {
namespace impl {

using d2d_path_geom_ptr = base::win::com_ptr<ID2D1PathGeometry>;
using d2d_geom_sink_ptr = base::win::com_ptr<ID2D1GeometrySink>;

}  // namepsace impl


enum class fill_mode {
	winding = D2D1_FILL_MODE_WINDING,
	even_odd = D2D1_FILL_MODE_ALTERNATE
};


class path {
public:
	path();
	explicit path(fill_mode mode);

	path(const path& rhs);
	path(path&& rhs) noexcept;

	void swap(path& other) noexcept;
	path& operator=(path rhs);

	void set_fill_mode(fill_mode mode);
	fill_mode get_fill_mode() const { return fill_mode_; }

	void move_to(const pointf& to);
	void line_to(const pointf& to);
	
	void quad_to(const pointf& ctrl, const pointf& end);
	void cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end);

	void arc_to(const pointf& end, const sizef& radius, float rotation_angle,
		bool large_arc = false, bool counter_clockwise = false);
	
	void close();

	void add_path(const path& other);

	path outline() const;
	path intersect(const path& other) const;

	void reset();

	float length() const;
	float area() const;

	rectf bounds() const;

	bool contains(const pointf& pt) const;

	pointf point_at(float dist) const;
	pointf tangent_at(float dist) const;

	const impl::d2d_path_geom_ptr& geom() const;

private:
	const impl::d2d_geom_sink_ptr& streaming_sink();
	const impl::d2d_geom_sink_ptr& figure_sink();

	void begin_figure();
	void end_figure() const;

	void ensure_has_sink();
	void ensure_closed() const;

	impl::d2d_path_geom_ptr recreate_geom();

	impl::d2d_path_geom_ptr geom_;

	mutable impl::d2d_geom_sink_ptr active_sink_;
	mutable bool in_figure_;

	fill_mode fill_mode_;
	
	pointf first_point_;
	pointf last_point_;
};


inline void swap(path& lhs, path& rhs) noexcept {
	lhs.swap(rhs);
}

}  // namespace gfx