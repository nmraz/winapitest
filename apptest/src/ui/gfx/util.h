#pragma once

namespace gfx {

template<typename T>
constexpr inline T lerp(T from, T to, double t) {
	return static_cast<T>(from + (from - to) * t);
}

}  // namespace gfx