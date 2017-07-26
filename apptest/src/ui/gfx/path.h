#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include <d2d1_1.h>

namespace gfx {
namespace impl {

class path_renderer;

using d2d_path_geom_ptr = base::win::com_ptr<ID2D1PathGeometry>;
using d2d_geom_sink_ptr = base::win::com_ptr<ID2D1GeometrySink>;

}  // namepsace impl


class path {
public:
	path();
	path(const path& rhs);
	path(path&& rhs) noexcept;

	void swap(path& other) noexcept;
	path& operator=(path rhs);
	
	void move_to(const pointf& to);
	void line_to(const pointf& to);
	void quad_to(const pointf& ctrl, const pointf& end);
	void cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end);
	
	void close();
	void outline();

	void flush();
	void reset();

	float length() const;
	float area() const;

	rectf bounds() const;

	bool contains(const pointf& pt) const;

	pointf point_at(float dist) const;
	pointf tangent_at(float dist) const;

private:
	friend impl::path_renderer;

	void begin_figure();
	void end_figure() const;

	void ensure_has_sink();

	void ensure_in_figure();
	void ensure_closed() const;

	impl::d2d_path_geom_ptr geom_;

	mutable impl::d2d_geom_sink_ptr active_sink_;
	mutable bool in_figure_;
	
	pointf first_point_;
	pointf last_point_;
};


inline void swap(path& lhs, path& rhs) noexcept {
	lhs.swap(rhs);
}

}  // namespace gfx