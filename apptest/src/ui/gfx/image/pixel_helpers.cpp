#include "pixel_helpers.h"

#include "base/assert.h"
#include <cstdint>
#include <cstring>
#include <utility>

namespace gfx {
namespace {

float to_float(std::uint8_t val) {
  return val / 255.f;
}

std::uint8_t from_float(float val) {
  return static_cast<std::uint8_t>(val * 255);
}


void read_channels(const void* pixel, pixel_format fmt, float& r, float& g, float& b, float& a) {
  const std::uint8_t* pixel8 = static_cast<const std::uint8_t*>(pixel);

  std::uint8_t r8 = 0, g8 = 0, b8 = 0, a8 = 0;
  switch (fmt) {
  case pixel_format::rgba8888:
    r8 = *pixel8++;
    g8 = *pixel8++;
    b8 = *pixel8++;
    a8 = *pixel8++;
    break;
  case pixel_format::bgra8888:
    b8 = *pixel8++;
    g8 = *pixel8++;
    r8 = *pixel8++;
    a8 = *pixel8++;
  default:
    NOTREACHED() << "Unknown pixel format";
  }

  r = to_float(r8);
  g = to_float(g8);
  b = to_float(b8);
  a = to_float(a8);
}

void write_chanels(void* pixel, pixel_format fmt, float r, float g, float b, float a) {
  std::uint8_t* pixel8 = static_cast<std::uint8_t*>(pixel);

  auto r8 = from_float(r);
  auto g8 = from_float(g);
  auto b8 = from_float(b);
  auto a8 = from_float(a);

  switch (fmt) {
  case gfx::pixel_format::rgba8888:
    *pixel8++ = r8;
    *pixel8++ = g8;
    *pixel8++ = b8;
    *pixel8++ = a8;
    break;
  case gfx::pixel_format::bgra8888:
    *pixel8++ = b8;
    *pixel8++ = g8;
    *pixel8++ = r8;
    *pixel8++ = a8;
    break;
  default:
    NOTREACHED() << "Unknown pixel format";
  }
}


void to_unpremul(alpha_mode amode, float& r, float& g, float& b, float& a) {
  switch (amode) {
  case gfx::alpha_mode::premul:
    if (a) {
      r /= a;
      g /= a;
      b /= a;
    }
    break;
  case gfx::alpha_mode::unpremul:
    break;  // nothing to do
  case gfx::alpha_mode::opaque:
    a = 1;  // ignore existing alpha
  default:
    NOTREACHED() << "Unknown alpha mode";
  }
}

void from_unpremul(alpha_mode amode, float& r, float& g, float& b, float& a) {
  switch (amode) {
  case gfx::alpha_mode::premul:
    r *= a;
    g *= a;
    b *= a;
    break;
  case gfx::alpha_mode::unpremul:
    break;  // nothing to do
  case gfx::alpha_mode::opaque:
    a = 0;  // any value is legal, alpha is ignored
  default:
    NOTREACHED() << "Unknown alpha mode";
  }
}

}  // namespace


color read_pixel(const void* pixel, pixel_format fmt, alpha_mode amode) {
  float r, g, b, a;
  read_channels(pixel, fmt, r, g, b, a);
  to_unpremul(amode, r, g, b, a);
  return { r, g, b, a };
}

void write_pixel(void* pixel, const color& col, pixel_format fmt, alpha_mode amode) {
  float r = col.r();
  float g = col.g();
  float b = col.b();
  float a = col.a();

  from_unpremul(amode, r, g, b, a);
  write_chanels(pixel, fmt, r, g, b, a);
}

}  // namespace gfx