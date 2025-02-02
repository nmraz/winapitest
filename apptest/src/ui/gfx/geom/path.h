#pragma once

#include "base/win/com_ptr.h"
#include "ui/gfx/stroke_style.h"
#include "ui/gfx/geom/point.h"
#include "ui/gfx/geom/rect.h"
#include "ui/gfx/geom/round_rect.h"
#include "ui/gfx/geom/size.h"
#include "ui/gfx/matrix.h"
#include <d2d1_1.h>
#include <initializer_list>
#include <mutex>
#include <variant>
#include <vector>

namespace gfx {
namespace impl {

using d2d_path_geom_ptr = base::win::com_ptr<ID2D1PathGeometry1>;
using d2d_geom_sink_ptr = base::win::com_ptr<ID2D1GeometrySink>;

}  // namepsace impl


enum class fill_mode {
  winding = D2D1_FILL_MODE_WINDING,
  even_odd = D2D1_FILL_MODE_ALTERNATE
};


enum class path_op {
  combine = D2D1_COMBINE_MODE_UNION,
  intersect = D2D1_COMBINE_MODE_INTERSECT,
  diff = D2D1_COMBINE_MODE_EXCLUDE,
  symmetric_diff = D2D1_COMBINE_MODE_XOR
};

enum class path_relation {
  disjoint = D2D1_GEOMETRY_RELATION_DISJOINT,
  contained = D2D1_GEOMETRY_RELATION_IS_CONTAINED,
  contains = D2D1_GEOMETRY_RELATION_CONTAINS,
  overlap = D2D1_GEOMETRY_RELATION_OVERLAP
};


enum class arc_size {
  large_arc = D2D1_ARC_SIZE_LARGE,
  small_arc = D2D1_ARC_SIZE_SMALL
};

enum class sweep_dir {
  clockwise = D2D1_SWEEP_DIRECTION_CLOCKWISE,
  counter_clockwise = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE
};


namespace path_verbs {

struct move {
  pointf to;
};

constexpr bool operator==(const move& lhs, const move& rhs) {
  return lhs.to == rhs.to;
}

constexpr bool operator!=(const move& lhs, const move& rhs) {
  return !(lhs == rhs);
}


struct close {};

constexpr bool operator==(const close&, const close&) {
  return true;
}

constexpr bool operator!=(const close&, const close&) {
  return false;
}


struct line {
  pointf to;
};

constexpr bool operator==(const line& lhs, const line& rhs) {
  return lhs.to == rhs.to;
}

constexpr bool operator!=(const line& lhs, const line& rhs) {
  return !(lhs == rhs);
}


struct quad {
  pointf ctrl;
  pointf end;
};

constexpr bool operator==(const quad& lhs, const quad& rhs) {
  return lhs.ctrl == rhs.ctrl && lhs.end == rhs.end;
}

constexpr bool operator!=(const quad& lhs, const quad& rhs) {
  return !(lhs == rhs);
}


struct cubic {
  pointf ctrl1;
  pointf ctrl2;
  pointf end;
};

constexpr bool operator==(const cubic& lhs, const cubic& rhs) {
  return lhs.ctrl1 == rhs.ctrl1 && lhs.ctrl2 == rhs.ctrl2 && lhs.end == rhs.end;
}

constexpr bool operator!=(const cubic& lhs, const cubic& rhs) {
  return !(lhs == rhs);
}


struct arc {
  pointf end;
  sizef radius;
  float rotation_angle;
  arc_size size;
  sweep_dir dir;
};

constexpr bool operator==(const arc& lhs, const arc& rhs) {
  return lhs.end == rhs.end && lhs.radius == rhs.radius && lhs.rotation_angle == rhs.rotation_angle
    && lhs.size == rhs.size && lhs.dir == rhs.dir;
}

constexpr bool operator!=(const arc& lhs, const arc& rhs) {
  return !(lhs == rhs);
}

}  // namespace path_verbs

using path_verb = std::variant<
  path_verbs::move,
  path_verbs::close,
  path_verbs::line,
  path_verbs::quad,
  path_verbs::cubic,
  path_verbs::arc
>;

class path {
public:
  using verb_list = std::vector<path_verb>;

  using iterator = verb_list::iterator;
  using const_iterator = verb_list::const_iterator;
  using reverse_iterator = verb_list::reverse_iterator;
  using const_reverse_iterator = verb_list::const_reverse_iterator;

  iterator begin() { return verbs().begin(); }
  const_iterator begin() const { return verbs().begin(); }
  const_iterator cbegin() const { return verbs().cbegin(); }

  iterator end() { return verbs().end(); }
  const_iterator end() const { return verbs().end(); }
  const_iterator cend() const { return verbs().cend(); }

  reverse_iterator rbegin() { return verbs().rbegin(); }
  const_reverse_iterator rbegin() const { return verbs().rbegin(); }
  const_reverse_iterator crbegin() const { return verbs().crbegin(); }

  reverse_iterator rend() { return verbs().rend(); }
  const_reverse_iterator rend() const { return verbs().rend(); }
  const_reverse_iterator crend() const { return verbs().crend(); }

  bool empty() const { return verbs().empty(); }
  std::size_t size() const { return verbs().size(); }

  path_verb& operator[](std::size_t idx);
  const path_verb& operator[](std::size_t idx) const;

  path_verb& front();
  const path_verb& front() const;

  path_verb& back();
  const path_verb& back() const;

  void push_back(path_verb verb) { verbs().push_back(std::move(verb)); }

  template<typename Iter>
  iterator insert(const_iterator where, Iter first, Iter last) { return verbs().insert(where, first, last); }
  iterator insert(const_iterator where, path_verb verb) { return verbs().insert(where, std::move(verb)); }

  iterator erase(const_iterator where) { return verbs().erase(where); }
  iterator erase(const_iterator first, const_iterator last) { return verbs().erase(first, last); }
  void clear() { verbs().clear(); }

  void swap(path& other) noexcept;


  void set_fill_mode(fill_mode mode);
  fill_mode get_fill_mode() const { return fill_mode_; }

  void move_to(const pointf& to);
  void close();
  void line_to(const pointf& to);
  void quad_to(const pointf& ctrl, const pointf& end);
  void cubic_to(const pointf& ctrl1, const pointf& ctrl2, const pointf& end);

  void arc_to(const pointf& end, const sizef& radius, float rotation_angle = 0.f,
    arc_size size = arc_size::small_arc, sweep_dir dir = sweep_dir::clockwise);
  void arc_to(const pointf& end, float radius, arc_size size = arc_size::small_arc,
    sweep_dir dir = sweep_dir::clockwise);

  void arc_to(const pointf& center, const sizef& radius, float start_angle,
    float sweep_angle, float rotation_angle = 0.f, bool force_move = false);
  void arc_to(const rectf& bounds, float start_angle, float sweep_angle,
    float rotation_angle = 0.f, bool force_move = false);
  
  void add_path(const path& other);
  void add_rect(const rectf& rc);
  void add_round_rect(const round_rect& rrect);
  void add_ellipse(const pointf& center, const sizef& radius, float rotation_angle = 0.f);
  void add_ellipse(const rectf& bounds, float rotation_angle = 0.f);


  path outline() const;
  path transform(const mat33f& tform) const;
  path combine(const path& other, path_op op) const;
  path widen(float stroke_width, const stroke_style& stroke = {}) const;

  float length() const;
  float area() const;
  rectf bounds() const;
  std::pair<pointf, pointf> point_tangent_at(float dist) const;

  path_relation compare(const path& other) const;
  bool contains(const pointf& pt) const;
  bool stroke_contains(const pointf& pt, float stroke_width, const stroke_style& stroke = {}) const;


  const impl::d2d_path_geom_ptr& d2d_geom() const;
  impl::d2d_geom_sink_ptr d2d_sink();

  void mark_dirty();
  void make_thread_safe() const;

  void stream_to(ID2D1GeometrySink* sink) const;

private:
  verb_list& verbs();
  const verb_list& verbs() const;

  verb_list verbs_;
  fill_mode fill_mode_ = fill_mode::winding;

  mutable impl::d2d_path_geom_ptr d2d_geom_;
};


inline void swap(path& lhs, path& rhs) noexcept {
  lhs.swap(rhs);
}

bool operator==(const path& lhs, const path& rhs);
bool operator!=(const path& lhs, const path& rhs);

}  // namespace gfx