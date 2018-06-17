#include "path.h"

#include "base/assert.h"
#include "base/span.h"
#include "base/win/com_impl.h"
#include "base/win/exception_boundary.h"
#include "base/win/last_error.h"
#include "ui/gfx/d2d/convs.h"
#include "ui/gfx/d2d/factories.h"
#include "ui/gfx/transform.h"
#include "ui/gfx/util.h"
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


class path_d2d_sink : public base::win::com_impl<ID2D1GeometrySink, ID2D1SimplifiedGeometrySink> {
public:
  using ptr = base::win::com_ptr<path_d2d_sink>;

  STDMETHOD_(void, SetFillMode)(D2D1_FILL_MODE mode) override;

  STDMETHOD_(void, BeginFigure)(D2D1_POINT_2F pt, D2D1_FIGURE_BEGIN) override;
  STDMETHOD_(void, EndFigure)(D2D1_FIGURE_END end_mode) override;

  STDMETHOD_(void, SetSegmentFlags)(D2D1_PATH_SEGMENT) override {}

  STDMETHOD_(void, AddLine)(D2D1_POINT_2F to) override;
  STDMETHOD_(void, AddBezier)(const D2D1_BEZIER_SEGMENT* bezier) override;
  STDMETHOD_(void, AddQuadraticBezier)(const D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) override;
  STDMETHOD_(void, AddArc)(const D2D1_ARC_SEGMENT* arc) override;

  STDMETHOD_(void, AddLines)(const D2D1_POINT_2F* points, UINT count) override;
  STDMETHOD_(void, AddBeziers)(const D2D1_BEZIER_SEGMENT* beziers, UINT count) override;
  STDMETHOD_(void, AddQuadraticBeziers)(const D2D1_QUADRATIC_BEZIER_SEGMENT* beziers, UINT count) override;

  STDMETHOD(Close)() override;

  static ptr create(path* p);

private:
  path_d2d_sink(path* p);

  template<typename F>
  void run_in_boundary(F&& f) noexcept {
    if (FAILED(hr_)) {
      return;
    }
    hr_ = base::win::exception_boundary(std::forward<F>(f));
  }

  path* path_;
  HRESULT hr_ = S_OK;
};


STDMETHODIMP_(void) path_d2d_sink::SetFillMode(D2D1_FILL_MODE mode) {
  run_in_boundary([&] {
    path_->set_fill_mode(static_cast<fill_mode>(mode));
  });
}

STDMETHODIMP_(void) path_d2d_sink::BeginFigure(D2D1_POINT_2F pt, D2D1_FIGURE_BEGIN) {
  run_in_boundary([&] {
    path_->move_to(impl::d2d_point_to_point(pt));
  });
}

STDMETHODIMP_(void) path_d2d_sink::EndFigure(D2D1_FIGURE_END end_mode) {
  run_in_boundary([&] {
    if (end_mode == D2D1_FIGURE_END_CLOSED) {
      path_->close();
    }
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddLine(D2D1_POINT_2F to) {
  run_in_boundary([&] {
    path_->line_to(impl::d2d_point_to_point(to));
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddBezier(const D2D1_BEZIER_SEGMENT* bezier) {
  run_in_boundary([&] {
    path_->cubic_to(
      impl::d2d_point_to_point(bezier->point1),
      impl::d2d_point_to_point(bezier->point2),
      impl::d2d_point_to_point(bezier->point3)
    );
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddQuadraticBezier(const D2D1_QUADRATIC_BEZIER_SEGMENT* bezier) {
  run_in_boundary([&] {
    path_->quad_to(
      impl::d2d_point_to_point(bezier->point1),
      impl::d2d_point_to_point(bezier->point2)
    );
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddArc(const D2D1_ARC_SEGMENT* arc) {
  run_in_boundary([&] {
    path_->arc_to(
      impl::d2d_point_to_point(arc->point),
      impl::d2d_size_to_size(arc->size),
      deg_to_rad(arc->rotationAngle),
      static_cast<arc_size>(arc->arcSize),
      static_cast<sweep_dir>(arc->sweepDirection)
    );
  });
}


STDMETHODIMP_(void) path_d2d_sink::AddLines(const D2D1_POINT_2F* points, UINT count) {
  run_in_boundary([&] {
    for (const auto& point : base::span{ points, count }) {
      path_->line_to(impl::d2d_point_to_point(point));
    }
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddBeziers(const D2D1_BEZIER_SEGMENT* beziers, UINT count) {
  run_in_boundary([&] {
    for (const auto& bezier : base::span{ beziers, count }) {
      path_->cubic_to(
        impl::d2d_point_to_point(bezier.point1),
        impl::d2d_point_to_point(bezier.point2),
        impl::d2d_point_to_point(bezier.point3)
      );
    }
  });
}

STDMETHODIMP_(void) path_d2d_sink::AddQuadraticBeziers(const D2D1_QUADRATIC_BEZIER_SEGMENT* beziers, UINT count) {
  run_in_boundary([&] {
    for (const auto& bezier : base::span{ beziers, count }) {
      path_->quad_to(
        impl::d2d_point_to_point(bezier.point1),
        impl::d2d_point_to_point(bezier.point2)
      );
    }
  });
}


STDMETHODIMP path_d2d_sink::Close() {
  return hr_;
}


path_d2d_sink::ptr path_d2d_sink::create(path* p) {
  return ptr(new path_d2d_sink(p));
}

path_d2d_sink::path_d2d_sink(path* p)
  : path_(p) {
}

}  // namespace


path::path(const path& rhs)
  : verbs_(rhs.verbs_)
  , fill_mode_(rhs.fill_mode_) {
  std::lock_guard hold(rhs.d2d_geom_lock_);
  d2d_geom_ = rhs.d2d_geom_;
}

path::path(path&& rhs) noexcept
  : verbs_(std::move(rhs.verbs_))
  , fill_mode_(rhs.fill_mode_)
  , d2d_geom_(std::move(rhs.d2d_geom_)) {  // no lock - safety is only guaranteed on const operations
}

path& path::operator=(path rhs) {
  rhs.swap(*this);
  return *this;
}


path_verb& path::operator[](std::size_t idx) {
  ASSERT(idx < size()) << "Out of bounds access";
  return verbs()[idx];
}

const path_verb& path::operator[](std::size_t idx) const {
  ASSERT(idx < size()) << "Out of bounds access";
  return verbs()[idx];
}


path_verb& path::front() {
  ASSERT(!empty()) << "Accessing nonexistent path verb";
  return verbs().front();
}

const path_verb& path::front() const {
  ASSERT(!empty()) << "Accessing nonexistent path verb";
  return verbs().front();
}

path_verb& path::back() {
  ASSERT(!empty()) << "Accessing nonexistent path verb";
  return verbs().back();
}

const path_verb& path::back() const {
  ASSERT(!empty()) << "Accessing nonexistent path verb";
  return verbs().back();
}


void path::swap(path& other) noexcept {
  using std::swap;

  // no lock - safety is only guaranteed on const operations
  swap(verbs_, other.verbs_);
  swap(fill_mode_, other.fill_mode_);
  swap(d2d_geom_, other.d2d_geom_);
}


void path::set_fill_mode(fill_mode mode) {
  if (mode != fill_mode_) {
    mark_dirty();
    fill_mode_ = mode;
  }
}


void path::move_to(const pointf& to) {
  push_back(path_verbs::move{ to });
}

void path::close() {
  push_back(path_verbs::close{});
}

void path::line_to(const pointf& to) {
  push_back(path_verbs::line{ to });
}

void path::quad_to(const pointf& ctrl, const pointf& end) {
  push_back(path_verbs::quad{ ctrl, end });
}

void path::cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end) {
  push_back(path_verbs::cubic{ ctrl1, ctrl2, end });
}

void path::arc_to(const pointf& end, const sizef& radius, float rotation_angle, arc_size size, sweep_dir dir) {
  push_back(path_verbs::arc{ end, radius, rotation_angle, size, dir });
}

void path::arc_to(const pointf& end, float radius, arc_size size, sweep_dir dir) {
  arc_to(end, { radius, radius }, 0.f, size, dir);
}


void path::arc_to(const pointf& center, const sizef& radius, float start_angle, float sweep_angle,
  float rotation_angle, bool force_move) {
  mat33f point_transform = transform::rotate(rotation_angle) * transform::translate(center.x(), center.y());

  pointf start = transform::apply(point_transform,
    point_for_angle(start_angle, radius.width(), radius.height()));

  if (empty() || force_move) {
    move_to(start);
  } else {
    line_to(start);
  }

  if (std::abs(sweep_angle) >= two_pi<float>) {
    // full ellipse: build with two arcs

    // `start - center` gives the start point `rel_start` for an ellipse centered at the origin;
    // 180 degrees opposite that is `-rel_start`, but transforming it back yields
    // `center - rel_start`, or `center - (start - center)`
    arc_to(2 * center - start, radius, rotation_angle);
    arc_to(start, radius, rotation_angle);
    return;
  }

  pointf end = transform::apply(point_transform,
    point_for_angle(start_angle + sweep_angle, radius.width(), radius.height()));

  arc_size size = std::abs(sweep_angle) > pi<float> ? arc_size::large_arc : arc_size::small_arc;
  sweep_dir dir = sweep_angle > 0 ? sweep_dir::clockwise : sweep_dir::counter_clockwise;

  arc_to(end, radius, rotation_angle, size, dir);
}

void path::arc_to(const rectf& bounds, float start_angle, float sweep_angle, float rotation_angle,
  bool force_move) {
  arc_to(bounds.center(), bounds.get_size() / 2, start_angle, sweep_angle, rotation_angle, force_move);
}


void path::add_path(const path& other) {
  if (other.empty()) {
    return;
  }

  // make sure that we don't keep extending some other contour
  if (!std::holds_alternative<path_verbs::move>(other.front())) {
    move_to({ 0, 0 });
  }

  insert(end(), other.verbs().begin(), other.verbs().end());
}

void path::add_rect(const rectf& rc) {
  move_to(rc.origin());
  line_to(rc.top_right());
  line_to(rc.bottom_right());
  line_to(rc.bottom_left());
  close();
}

void path::add_round_rect(const round_rect& rrect) {
  const rectf& bounds = rrect.bounds();

  // start drawing from the top of the bottom-left corner
  move_to({ bounds.x(), bounds.bottom() - rrect.radius(rect_corner::bottom_left).height() });

  arc_to({
    bounds.x() + rrect.radius(rect_corner::top_left).width(),
    bounds.y() + rrect.radius(rect_corner::top_left).height()
  }, rrect.radius(rect_corner::top_left), pi<float>, half_pi<float>);
  arc_to({
    bounds.right() - rrect.radius(rect_corner::top_right).width(),
    bounds.y() + rrect.radius(rect_corner::top_right).height()
  }, rrect.radius(rect_corner::top_right), -half_pi<float>, half_pi<float>);
  arc_to({
    bounds.right() - rrect.radius(rect_corner::bottom_right).width(),
    bounds.bottom() - rrect.radius(rect_corner::bottom_right).height()
  }, rrect.radius(rect_corner::bottom_right), 0, half_pi<float>);
  arc_to({
    bounds.x() + rrect.radius(rect_corner::bottom_left).width(),
    bounds.y() - rrect.radius(rect_corner::bottom_left).height()
  }, rrect.radius(rect_corner::bottom_left), half_pi<float>, half_pi<float>);
}

void path::add_ellipse(const pointf& center, const sizef& radius, float rotation_angle) {
  mat33f rotation_transform = transform::rotate(rotation_angle);
  pointf rel_start = transform::apply(rotation_transform, { radius.width(), 0 });

  pointf start = center + rel_start;
  pointf end = center - rel_start;

  move_to(start);
  arc_to(end, radius, rotation_angle);
  arc_to(start, radius, rotation_angle);
}

void path::add_ellipse(const rectf& bounds, float rotation_angle) {
  add_ellipse(bounds.center(), bounds.get_size() / 2, rotation_angle);
}


path path::outline() const {
  path ret;
  base::win::throw_if_failed(
    d2d_geom()->Outline(nullptr, ret.d2d_sink().get()),
    "Failed to compute path outline"
  );
  return ret;
}

path path::transform(const mat33f& tform) const {
  path ret;
  base::win::throw_if_failed(
    d2d_geom()->Simplify(
      D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES,
      impl::mat33_to_d2d_mat32(tform),
      ret.d2d_sink().get()
    ),
    "Failed to transform path"
  );
  return ret;
}

path path::combine(const path& other, path_op op) const {
  path ret;
  base::win::throw_if_failed(
    d2d_geom()->CombineWithGeometry(
      other.d2d_geom().get(),
      static_cast<D2D1_COMBINE_MODE>(op),
      nullptr,
      ret.d2d_sink().get()
    ),
    "Failed to combine paths"
  );
  return ret;
}

path path::widen(float stroke_width, const stroke_style& stroke) const {
  path ret;
  base::win::throw_if_failed(
    d2d_geom()->Widen(
      stroke_width,
      stroke.d2d_stroke_style().get(),
      nullptr,
      ret.d2d_sink().get()
    ),
    "Failed to widen path"
  );
  return ret;
}


float path::length() const {
  float ret;
  base::win::throw_if_failed(
    d2d_geom()->ComputeLength(nullptr, &ret),
    "Failed to compute path length"
  );
  return ret;
}

float path::area() const {
  float ret;
  base::win::throw_if_failed(
    d2d_geom()->ComputeArea(nullptr, &ret),
    "Failed to compute path area"
  );
  return ret;
}

rectf path::bounds() const {
  D2D1_RECT_F ret;
  base::win::throw_if_failed(
    d2d_geom()->GetBounds(nullptr, &ret),
    "Failed to compute path bounds"
  );
  return impl::d2d_rect_to_rect(ret);
}

std::pair<pointf, pointf> path::point_tangent_at(float dist) const {
  D2D1_POINT_2F pt, tangent;
  base::win::throw_if_failed(
    d2d_geom()->ComputePointAtLength(dist, nullptr, &pt, &tangent),
    "Failed to compute point information on path"
  );
  return { impl::d2d_point_to_point(pt), impl::d2d_point_to_point(tangent) };
}


path_relation path::compare(const path& other) const {
  D2D1_GEOMETRY_RELATION ret;
  base::win::throw_if_failed(
    d2d_geom()->CompareWithGeometry(other.d2d_geom().get(), nullptr, &ret),
    "Failed to compare paths"
  );
  return static_cast<path_relation>(ret);
}

bool path::contains(const pointf& pt) const {
  BOOL ret;
  base::win::throw_if_failed(
    d2d_geom()->FillContainsPoint(impl::point_to_d2d_point(pt), nullptr, &ret),
    "Failed to hit-test path"
  );
  return ret;
}

bool path::stroke_contains(const pointf& pt, float stroke_width, const stroke_style& stroke) const {
  BOOL ret;
  base::win::throw_if_failed(
    d2d_geom()->StrokeContainsPoint(
      impl::point_to_d2d_point(pt),
      stroke_width,
      stroke.d2d_stroke_style().get(),
      nullptr,
      &ret
    ),
    "Failed to hit-test path stroke"
  );
  return ret;
}


const impl::d2d_path_geom_ptr& path::d2d_geom() const {
  std::lock_guard hold(d2d_geom_lock_);

  if (!d2d_geom_) {
    d2d_geom_ = create_path_geom();
    impl::d2d_geom_sink_ptr sink;
    base::win::throw_if_failed(d2d_geom_->Open(sink.addr()), "Failed to open path sink");
    stream_to(sink.get());
  }
  return d2d_geom_;  // will not be mutated again until a non-const operation is performed
}

impl::d2d_geom_sink_ptr path::d2d_sink() {
  return path_d2d_sink::create(this);
}


void path::mark_dirty() {
  d2d_geom_ = nullptr;  // no lock - safety is only guaranteed on const operations
}

void path::stream_to(ID2D1GeometrySink* sink) const {
  struct streaming_visitor {
    streaming_visitor(ID2D1GeometrySink* sink)
      : sink(sink) {
    }

    void ensure_in_figure() {
      if (!in_figure) {
        (*this)(path_verbs::move{ last_move_to });
      }
    }

    void operator()(const path_verbs::move& move) {
      if (in_figure) {
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
      }

      sink->BeginFigure(impl::point_to_d2d_point(move.to), D2D1_FIGURE_BEGIN_FILLED);

      last_move_to = move.to;
      in_figure = true;
    }

    void operator()(const path_verbs::close&) {
      if (!in_figure) {
        return;
      }

      sink->EndFigure(D2D1_FIGURE_END_CLOSED);

      in_figure = false;
    }

    void operator()(const path_verbs::line& line) {
      ensure_in_figure();
      sink->AddLine(impl::point_to_d2d_point(line.to));
    }

    void operator()(const path_verbs::quad& quad) {
      ensure_in_figure();
      sink->AddQuadraticBezier({
        impl::point_to_d2d_point(quad.ctrl),
        impl::point_to_d2d_point(quad.end)
      });
    }

    void operator()(const path_verbs::cubic& cubic) {
      ensure_in_figure();
      sink->AddBezier({
        impl::point_to_d2d_point(cubic.ctrl1),
        impl::point_to_d2d_point(cubic.ctrl2),
        impl::point_to_d2d_point(cubic.end)
      });
    }

    void operator()(const path_verbs::arc& arc) {
      ensure_in_figure();
      sink->AddArc({
        impl::point_to_d2d_point(arc.end),
        impl::size_to_d2d_size(arc.radius),
        rad_to_deg(arc.rotation_angle),
        static_cast<D2D1_SWEEP_DIRECTION>(arc.dir),
        static_cast<D2D1_ARC_SIZE>(arc.size)
      });
    }

    void close() {
      if (in_figure) {
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
      }
      base::win::throw_if_failed(sink->Close(), "Failed to stream path to sink");
    }


    ID2D1GeometrySink* sink;
    bool in_figure = false;

    pointf last_move_to;
  };

  sink->SetFillMode(static_cast<D2D1_FILL_MODE>(fill_mode_));

  streaming_visitor visitor(sink);
  for (const auto& verb : *this) {
    std::visit(visitor, verb);
  }

  visitor.close();
}


// PRIVATE

path::verb_list& path::verbs() {
  // the path may be modified externally, so discard our current geometry
  mark_dirty();
  return verbs_;
}

const path::verb_list& path::verbs() const {
  return verbs_;
}


// NONMEMBER

bool operator==(const path& lhs, const path& rhs) {
  return lhs.get_fill_mode() == rhs.get_fill_mode() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

bool operator!=(const path& lhs, const path& rhs) {
  return !(lhs == rhs);
}

}  // namepsace gfx