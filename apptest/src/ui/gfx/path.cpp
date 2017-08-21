#include "path.h"

#include "base/assert.h"
#include "base/win/last_error.h"
#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/d2d/factories.h"
#include <utility>

namespace gfx {
namespace {

auto create_path_geom() {
	impl::d2d_path_geom_ptr geom;

	base::win::throw_if_failed(
		impl::get_d2d_factory()->CreatePathGeometry(geom.addr()),
		"Failed to create path geometry"
	);
	return geom;
}

auto create_sink(const impl::d2d_path_geom_ptr& geom, fill_mode mode) {
	impl::d2d_geom_sink_ptr sink;

	base::win::throw_if_failed(geom->Open(sink.addr()), "Failed to create path sink");
	sink->SetFillMode(static_cast<D2D1_FILL_MODE>(mode));
	return sink;
}


void close_sink(impl::d2d_geom_sink_ptr& sink) {
	base::win::throw_if_failed(sink->Close(), "Failed to close path sink");
	sink = nullptr;
}

void stream_geom(const impl::d2d_path_geom_ptr& path, const impl::d2d_geom_sink_ptr& sink) {
	base::win::throw_if_failed(path->Stream(sink.get()), "Failed to copy path");
}

}  // namespace


path::path()
	: path(fill_mode::winding) {
}

path::path(fill_mode mode)
	: in_figure_(false)
	, fill_mode_(mode) {
	recreate_geom();
}

path::path(const path& rhs)
	: geom_(rhs.geom())
	, in_figure_(false)
	, fill_mode_(rhs.fill_mode_)
	, first_point_(rhs.first_point_)
	, last_point_(rhs.last_point_) {
}

path::path(path&& rhs) noexcept
	: geom_(std::move(rhs.geom_))
	, active_sink_(std::move(rhs.active_sink_))
	, in_figure_(rhs.in_figure_)
	, fill_mode_(rhs.fill_mode_)
	, first_point_(rhs.first_point_)
	, last_point_(rhs.last_point_) {
}


void path::swap(path& other) noexcept {
	using std::swap;

	swap(geom_, other.geom_);
	swap(active_sink_, other.active_sink_);
	swap(in_figure_, other.in_figure_);

	swap(fill_mode_, other.fill_mode_);

	swap(first_point_, other.first_point_);
	swap(last_point_, other.last_point_);
}

path& path::operator=(path rhs) {
	rhs.swap(*this);
	return *this;
}


void path::set_fill_mode(fill_mode mode) {
	bool mode_changed = mode != fill_mode_;

	fill_mode_ = mode;

	if (mode_changed && active_sink_) {
		// close and reopen the sink for fill mode to take effect
		ensure_closed();
		ensure_has_sink();
	}
}


void path::move_to(const pointf& to) {
	ensure_has_sink();
	end_figure(D2D1_FIGURE_END_OPEN);

	first_point_ = last_point_ = to;
	
	begin_figure();
}

void path::line_to(const pointf& to) {
	sink()->AddLine(impl::point_to_d2d_point(to));
	last_point_ = to;
}

void path::quad_to(const pointf& ctrl, const pointf& end) {
	D2D1_QUADRATIC_BEZIER_SEGMENT segment = {
		impl::point_to_d2d_point(ctrl),
		impl::point_to_d2d_point(end)
	};

	sink()->AddQuadraticBezier(segment);
	last_point_ = end;
}

void path::cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end) {
	D2D1_BEZIER_SEGMENT segment = {
		impl::point_to_d2d_point(ctrl1),
		impl::point_to_d2d_point(ctrl2),
		impl::point_to_d2d_point(end)
	};

	sink()->AddBezier(segment);
	last_point_ = end;
}

void path::arc_to(const pointf& end, const sizef& radius, float rotation_angle, bool large_arc, bool counter_clockwise) {
	D2D1_ARC_SEGMENT segment = {
		impl::point_to_d2d_point(end),
		impl::size_to_d2d_size(radius),
		rad_to_deg(rotation_angle),
		counter_clockwise ? D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE : D2D1_SWEEP_DIRECTION_CLOCKWISE,
		large_arc ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL
	};

	sink()->AddArc(segment);
	last_point_ = end;
}


void path::close() {
	last_point_ = first_point_;
	end_figure(D2D1_FIGURE_END_CLOSED);
}

void path::outline() {
	auto old_geom = recreate_geom();
	base::win::throw_if_failed(old_geom->Outline(nullptr, sink().get()), "Failed to compute path outline");
}

void path::intersect(const path& other) {
	auto old_geom = recreate_geom();

	base::win::throw_if_failed(
		old_geom->CombineWithGeometry(other.geom().get(), D2D1_COMBINE_MODE_INTERSECT, nullptr, sink().get()),
		"Failed to intersect geometry"
	);
}


void path::reset() {
	path(fill_mode_).swap(*this);
}


float path::length() const {
	FLOAT length;
	base::win::throw_if_failed(geom()->ComputeLength(nullptr, &length), "Failed to compute path length");
	return length;
}

float path::area() const {
	FLOAT area;
	base::win::throw_if_failed(geom()->ComputeArea(nullptr, &area), "Failed to compute path area");
	return area;
}


rectf path::bounds() const {
	D2D1_RECT_F bounds;
	base::win::throw_if_failed(geom()->GetBounds(nullptr, &bounds), "Failed to compute path bounds");
	return impl::d2d_rect_to_rect(bounds);
}


bool path::contains(const pointf& pt) const {
	BOOL contained;
	base::win::throw_if_failed(
		geom()->FillContainsPoint(impl::point_to_d2d_point(pt), nullptr, &contained),
		"Failed to hit test path"
	);
	return static_cast<bool>(contained);
}


pointf path::point_at(float dist) const {
	D2D1_POINT_2F pt;
	base::win::throw_if_failed(
		geom()->ComputePointAtLength(dist, nullptr, &pt, nullptr),
		"Failed to find point along path"
	);
	return impl::d2d_point_to_point(pt);
}

pointf path::tangent_at(float dist) const {
	D2D1_POINT_2F tangent;
	base::win::throw_if_failed(
		geom()->ComputePointAtLength(dist, nullptr, nullptr, &tangent),
		"Failed to find tangent to path"
	);
	return impl::d2d_point_to_point(tangent);
}


// PRIVATE

const impl::d2d_path_geom_ptr& path::geom() const {
	ensure_closed();
	return geom_;
}

const impl::d2d_geom_sink_ptr& path::sink() {
	ensure_has_sink();
	return active_sink_;
}


void path::begin_figure() {
	if (!in_figure_) {
		active_sink_->BeginFigure(impl::point_to_d2d_point(last_point_), D2D1_FIGURE_BEGIN_FILLED);
		in_figure_ = true;
	}
}

void path::end_figure(D2D1_FIGURE_END end_mode) const {
	if (in_figure_) {
		active_sink_->EndFigure(end_mode);
		in_figure_ = false;
	}
}


void path::ensure_has_sink() {
	if (!active_sink_) {
		// the active sink has been closed - we need a new sink and a new geometry
		auto old_geom = recreate_geom();
		stream_geom(old_geom, sink());
	}
}


void path::ensure_in_figure() {
	ensure_has_sink();
	begin_figure();
}

void path::ensure_closed() const {
	if (active_sink_) {
		end_figure(D2D1_FIGURE_END_OPEN);
		close_sink(active_sink_);
	}
}


impl::d2d_path_geom_ptr path::recreate_geom() {
	ensure_closed();

	auto new_geom = create_path_geom();
	active_sink_ = create_sink(new_geom, fill_mode_);
	
	return std::exchange(geom_, std::move(new_geom));
}

}  // namepsace gfx