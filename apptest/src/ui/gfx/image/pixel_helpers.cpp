#include "pixel_helpers.h"

#include <cstdint>
#include <utility>

namespace gfx {
namespace {

template<typename T>
constexpr T mask_for(std::size_t bits) {
  return (T(1) << bits) - 1;
}

template<std::size_t I, std::size_t... Seq, typename... T>
constexpr std::size_t partial_sum_after(T... vals) {
  return ((Seq > I ? vals : 0) + ...);
}


template<std::size_t... Bits, typename P, typename... U, std::size_t... I>
void unpack_helper(std::index_sequence<I...>, P packed, U&... unpacked) {
  ((unpacked = static_cast<U>((packed >> partial_sum_after<I, I...>(Bits...)) & mask_for<P>(Bits))), ...);
}

template<std::size_t... Bits, typename P, typename... U>
void unpack(P packed, U&... unpacked) {
  static_assert(sizeof...(Bits) == sizeof...(U));
  unpack_helper<Bits...>(std::index_sequence_for<U...>{}, packed, unpacked...);
}

template<std::size_t... Bits, typename P, typename... U, std::size_t... I>
P pack_helper(std::index_sequence<I...>, U... unpacked) {
  return (((unpacked & mask_for<P>(Bits)) << partial_sum_after<I, I...>(Bits...)) | ...);
}

template<typename P, std::size_t... Bits, typename... U>
P pack(U... unpacked) {
  static_assert(sizeof...(Bits) == sizeof...(U));
  return pack_helper<Bits, P>(std::index_sequence_for<U...>{}, unpacked...);
}

}  // namespace

}  // namespace gfx