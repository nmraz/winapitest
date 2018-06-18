#include "pixel_helpers.h"

#include "base/assert.h"
#include <cstdint>
#include <cstring>
#include <utility>

namespace gfx {
namespace {

template<typename T>
constexpr T max_val(std::size_t bits) {
  return (T(1) << bits) - 1;
}

template<std::size_t I, std::size_t... Seq, typename... T>
constexpr std::size_t partial_sum_after(T... vals) {
  return ((Seq > I ? vals : 0) + ...);
}


template<std::size_t... Bits, typename P, typename... U, std::size_t... I>
void unpack_helper(std::index_sequence<I...>, P packed, U&... unpacked) {
  ((unpacked = static_cast<U>((packed >> partial_sum_after<I, I...>(Bits...)) & max_val<P>(Bits))), ...);
}

template<std::size_t... Bits, typename P, typename... U>
void unpack(P packed, U&... unpacked) {
  static_assert(sizeof...(Bits) == sizeof...(U));
  unpack_helper<Bits...>(std::index_sequence_for<U...>{}, packed, unpacked...);
}

template<typename P, std::size_t... Bits, typename... U, std::size_t... I>
P pack_helper(std::index_sequence<I...>, U... unpacked) {
  return (((unpacked & max_val<P>(Bits)) << partial_sum_after<I, I...>(Bits...)) | ...);
}

template<typename P, std::size_t... Bits, typename... U>
P pack(U... unpacked) {
  static_assert(sizeof...(Bits) == sizeof...(U));
  return pack_helper<P, Bits...>(std::index_sequence_for<U...>{}, unpacked...);
}


template<typename T, std::size_t Bits = sizeof(T) * CHAR_BIT>
float to_float(T val) {
  return static_cast<float>(val) / max_val<T>(Bits);
}

template<typename T, std::size_t Bits = sizeof(T) * CHAR_BIT>
T from_float(float val) {
  return static_cast<T>(val * max_val<T>(Bits));
}


void read_channels(const void* pixel, pixel_format fmt, float& r, float& g, float& b, float& a) {
  std::uint32_t packed_pixel;
  // casting here could violate strict aliasing, use memcpy instead
  std::memcpy(&packed_pixel, pixel, sizeof(std::uint32_t));

  std::uint8_t r8 = 0, g8 = 0, b8 = 0, a8 = 0;
  switch (fmt) {
  case pixel_format::rgba8888:
    unpack<8, 8, 8, 8>(packed_pixel, r8, g8, b8, a8);
    break;
  case pixel_format::bgra8888:
    unpack<8, 8, 8, 8>(packed_pixel, b8, g8, r8, a8);
  default:
    NOTREACHED() << "Unknown pixel format";
  }

  r = to_float(r8);
  g = to_float(g8);
  b = to_float(b8);
  a = to_float(a8);
}

void write_chanels(void* pixel, pixel_format fmt, float r, float g, float b, float a) {
  auto r8 = from_float<std::uint8_t>(r);
  auto g8 = from_float<std::uint8_t>(g);
  auto b8 = from_float<std::uint8_t>(b);
  auto a8 = from_float<std::uint8_t>(a);

  std::uint32_t packed_pixel;

  switch (fmt) {
  case gfx::pixel_format::rgba8888:
    packed_pixel = pack<std::uint32_t, 8, 8, 8, 8>(r8, g8, b8, a8);
    break;
  case gfx::pixel_format::bgra8888:
    packed_pixel = pack<std::uint32_t, 8, 8, 8, 8>(b8, g8, r8, a8);
    break;
  default:
    NOTREACHED() << "Unknown pixel format";
  }

  // casting here could violate strict aliasing, use memcpy instead
  std::memcpy(pixel, &packed_pixel, sizeof(std::uint32_t));
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

}  // namespace gfx