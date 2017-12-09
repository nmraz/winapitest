#pragma once

#include <algorithm>
#include <type_traits>

namespace gfx {
namespace impl {

template<typename T>
using enable_if_floating = std::enable_if_t<std::is_floating_point_v<T>, int>;

template<typename T>
using enable_if_integral = std::enable_if_t<std::is_integral_v<T>, int>;


template<typename T>
constexpr float clamp_channel(T val) {
  return std::clamp(static_cast<float>(val), 0.f, 1.f);
}

}  // namespace impl


class color {
public:
  constexpr color()
    : color{ 0.f, 0.f, 0.f, 0.f } {
  }

  template<typename T, impl::enable_if_floating<T> = 0>
  constexpr color(T r, T g, T b, T a = static_cast<T>(1.f));

  template<typename T, impl::enable_if_integral<T> = 0>
  constexpr color(T r, T g, T b, T a = static_cast<T>(255))
    : color{ r / 255.f, g / 255.f, b / 255.f, a / 255.f } {
  }

  constexpr float r() const { return r_; }
  constexpr float g() const { return g_; }
  constexpr float b() const { return b_; }
  constexpr float a() const { return a_; }

  template<typename T, impl::enable_if_floating<T> = 0>
  constexpr void set_r(T r);
  template<typename T, impl::enable_if_integral<T> = 0>
  constexpr void set_r(T r) { set_r(r / 255.f); }

  template<typename T, impl::enable_if_floating<T> = 0>
  constexpr void set_g(T g);
  template<typename T, impl::enable_if_integral<T> = 0>
  constexpr void set_g(T g) { set_g(g / 255.f); }

  template<typename T, impl::enable_if_floating<T> = 0>
  constexpr void set_b(T b);
  template<typename T, impl::enable_if_integral<T> = 0>
  constexpr void set_b(T b) { set_b(b / 255.f); }

  template<typename T, impl::enable_if_floating<T> = 0>
  constexpr void set_a(T a);
  template<typename T, impl::enable_if_integral<T> = 0>
  constexpr void set_a(T a) { set_a(a / 255.f); }

private:
  float r_;
  float g_;
  float b_;
  float a_;
};


template<typename T, impl::enable_if_floating<T>>
constexpr color::color(T r, T g, T b, T a)
  : r_(impl::clamp_channel(r))
  , g_(impl::clamp_channel(g))
  , b_(impl::clamp_channel(b))
  , a_(impl::clamp_channel(a)) {
}

template<typename T, impl::enable_if_floating<T>>
constexpr void color::set_r(T r) {
  r_ = impl::clamp_channel(r);
}

template<typename T, impl::enable_if_floating<T>>
constexpr void color::set_g(T g) {
  g_ = impl::clamp_channel(g);
}

template<typename T, impl::enable_if_floating<T>>
constexpr void color::set_b(T b) {
  b_ = impl::clamp_channel(b);
}

template<typename T, impl::enable_if_floating<T>>
constexpr void color::set_a(T a) {
  a_ = impl::clamp_channel(a);
}


constexpr bool operator==(const color& lhs, const color& rhs) {
  return lhs.r() == rhs.r() && lhs.g() == rhs.g()
    && lhs.b() == rhs.b() && lhs.a() == rhs.a();
}

constexpr bool operator!=(const color& lhs, const color& rhs) {
  return !(lhs == rhs);
}

template<typename T, impl::enable_if_floating<T> = 0>
constexpr color& operator*=(color& col, T val) {
  col.set_a(col.a() * val);
  return col;
}

template<typename T, impl::enable_if_integral<T> = 0>
constexpr color& operator*=(color& col, T val) {
  return col *= val / 255.f;
}

template<typename T>
constexpr color operator*(const color& col, T val) {
  color ret = col;
  return ret *= val;
}

template<typename T>
constexpr color operator*(T val, const color& col) {
  return col * val;
}


color alpha_blend(const color& foreground, const color& background, float alpha);

template<typename T, impl::enable_if_floating<T> = 0>
color alpha_blend(const color& foreground, const color& background, T alpha) {
  return alpha_blend(foreground, background, static_cast<float>(alpha));
}
template<typename T, impl::enable_if_integral<T> = 0>
color alpha_blend(const color& foreground, const color& background, T alpha) {
  return alpha_blend(foreground, background, alpha / 255.f);
}

color lerp(const color& from, const color& to, double t);


namespace color_constants {

constexpr color transparent{};
constexpr color white{ 255, 255, 255 };
constexpr color black{ 0, 0, 0 };

constexpr color red{ 255, 0, 0 };
constexpr color green{ 0, 255, 0 };
constexpr color blue{ 0, 0, 255 };

constexpr color yellow{ 255, 255, 0 };
constexpr color magenta{ 255, 0, 255 };
constexpr color cyan{ 0, 255, 255 };

constexpr color orange{ 255, 69, 0 };
constexpr color violet{ 238, 130, 238 };
constexpr color turquoise{ 64, 244, 208 };
constexpr color brown{ 165, 42, 42 };

}  // namespace color_constants
}  // namespace gfx