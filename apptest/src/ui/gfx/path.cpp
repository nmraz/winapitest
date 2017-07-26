#include "path.h"

#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/d2d/error.h"
#include "ui/gfx/d2d/factories.h"
#include <utility>

namespace gfx {
namespace {

auto create_path_geom() {
	impl::d2d_path_geom_ptr geom;

	impl::throw_if_failed(
		impl::get_d2d_factory()->CreatePathGeometry(geom.addr()),
		"Failed to create path geometry"
	);
	return geom;
}


auto create_sink(const impl::d2d_path_geom_ptr& geom) {
	impl::d2d_geom_sink_ptr sink;

	impl::throw_if_failed(geom->Open(sink.addr()), "Failed to create path sink");
	return sink;
}

void close_sink(impl::d2d_geom_sink_ptr& sink) {
	impl::throw_if_failed(sink->Close(), "Failed to close path sink");
	sink = nullptr;
}


void stream_geom(const impl::d2d_path_geom_ptr& path, const impl::d2d_geom_sink_ptr& sink) {
	impl::throw_if_failed(path->Stream(sink.get()), "Failed to copy path");
}

}  // namespace


path::path()
	: geom_(create_path_geom())
	, in_figure_(false) {
	active_sink_ = create_sink(geom_);
}

path::path(const path& rhs)
	: path() {
	stream_geom(rhs.geom_, active_sink_);

	first_point_ = rhs.first_point_;
	last_point_ = rhs.last_point_;
}

path::path(path&& rhs) noexcept
	: geom_(std::move(rhs.geom_))
	, active_sink_(std::move(rhs.active_sink_))
	, in_figure_(rhs.in_figure_)
	, first_point_(rhs.first_point_)
	, last_point_(rhs.last_point_) {
}


void path::swap(path& other) {
	geom_.swap(other.geom_);
	active_sink_.swap(other.active_sink_);
	std::swap(first_point_, other.first_point_);
}

path& path::operator=(path rhs) {
	rhs.swap(*this);
	return *this;
}


void path::move_to(const pointf& to) {
	ensure_has_sink();
	end_figure();

	first_point_ = last_point_ = to;
	
	begin_figure();
}

void path::line_to(const pointf& to) {
	ensure_in_figure();

	last_point_ = to;
	active_sink_->AddLine(impl::point_to_d2d_point(to));
}

void path::quad_to(const pointf& ctrl, const pointf& end) {
	ensure_in_figure();

	last_point_ = end;

	D2D1_QUADRATIC_BEZIER_SEGMENT segment = {
		impl::point_to_d2d_point(ctrl),
		impl::point_to_d2d_point(end)
	};
	active_sink_->AddQuadraticBezier(segment);
}

void path::cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end) {
	ensure_in_figure();

	last_point_ = end;

	D2D1_BEZIER_SEGMENT segment = {
		impl::point_to_d2d_point(ctrl1),
		impl::point_to_d2d_point(ctrl2),
		impl::point_to_d2d_point(end)
	};
	active_sink_->AddBezier(segment);
}


void path::close() {
	if (last_point_ != first_point_) {
		line_to(first_point_);
	}
}

void path::outline() {
	ensure_closed();

	auto new_geom = create_path_geom();
	auto new_sink = create_sink(new_geom);

	impl::throw_if_failed(geom_->Outline(nullptr, new_sink.get()), "Failed to compute path outline");

	geom_.swap(new_geom);
	active_sink_.swap(new_sink);
}


void path::flush() {
	ensure_closed();
}

void path::reset() {
	geom_ = create_path_geom();

	active_sink_ = create_sink(geom_);
	in_figure_ = false;

	first_point_ = last_point_ = pointf();
}


float path::length() const {
	ensure_closed();

	FLOAT length;
	impl::throw_if_failed(geom_->ComputeLength(nullptr, &length), "Failed to compute path length");
	return length;
}

float path::area() const {
	ensure_closed();

	FLOAT area;
	impl::throw_if_failed(geom_->ComputeArea(nullptr, &area), "Failed to compute path area");
	return area;
}


rectf path::bounds() const {
	ensure_closed();

	D2D1_RECT_F bounds;
	impl::throw_if_failed(geom_->GetBounds(nullptr, &bounds), "Failed to compute path bounds");
	return impl::d2d_rect_to_rect(bounds);
}


bool path::contains(const pointf& pt) const {
	ensure_closed();

	BOOL contained;
	impl::throw_if_failed(
		geom_->FillContainsPoint(impl::point_to_d2d_point(pt), nullptr, &contained),
		"Failed to hit test path"
	);
	return static_cast<bool>(contained);
}


pointf path::point_at(float dist) const {
	ensure_closed();

	D2D1_POINT_2F pt;
	impl::throw_if_failed(
		geom_->ComputePointAtLength(dist, nullptr, &pt, nullptr),
		"Failed to find point along path"
	);
	return impl::d2d_point_to_point(pt);
}

pointf path::tangent_at(float dist) const {
	ensure_closed();

	D2D1_POINT_2F tangent;
	impl::throw_if_failed(
		geom_->ComputePointAtLength(dist, nullptr, nullptr, &tangent),
		"Failed to find tangent to path"
	);
	return impl::d2d_point_to_point(tangent);
}


// PRIVATE

void path::begin_figure() {
	if (!in_figure_) {
		active_sink_->BeginFigure(impl::point_to_d2d_point(last_point_), D2D1_FIGURE_BEGIN_FILLED);
		in_figure_ = true;
	}
}

void path::end_figure() const {
	if (in_figure_) {
		active_sink_->EndFigure(D2D1_FIGURE_END_OPEN);
		in_figure_ = false;
	}
}


void path::ensure_has_sink() {
	if (!active_sink_) {
		// the active sink has been closed - we need a new sink and a new geometry
		auto new_geom = create_path_geom();
		auto new_sink = create_sink(new_geom);

		stream_geom(geom_, new_sink);

		geom_.swap(new_geom);
		active_sink_.swap(new_sink);
	}
}


void path::ensure_in_figure() {
	ensure_has_sink();
	begin_figure();
}

void path::ensure_closed() const {
	if (active_sink_) {
		end_figure();
		close_sink(active_sink_);
	}
}

}  // namepsace gfx