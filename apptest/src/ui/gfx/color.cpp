#include "color.h"

#include <algorithm>

namespace gfx {

color lerp(color from, color to, double t) {
	t = std::clamp(t, 0.0, 1.0);

	int r = static_cast<int>(from.r + (static_cast<int>(to.r) - from.r) * t);
	int g = static_cast<int>(from.g + (static_cast<int>(to.g) - from.g) * t);
	int b = static_cast<int>(from.b + (static_cast<int>(to.b) - from.b) * t);
	int a = static_cast<int>(from.a + (static_cast<int>(to.a) - from.a) * t);

	return {
		static_cast<std::uint8_t>(r),
		static_cast<std::uint8_t>(g),
		static_cast<std::uint8_t>(b),
		static_cast<std::uint8_t>(a)
	};
}

color alpha_blend(color foreground, color background, std::uint8_t alpha) {
	if (alpha == 0) {
		return background;
	}
	if (alpha == 255) {
		return foreground;
	}

	double norm = (foreground.a * alpha + background.a * (255 - alpha)) / 255.0;
	if (norm == 0.0) {
		return color_constants::transparent;  // both colors are transparent
	}

	double fg_weight = foreground.a * alpha / norm;
	double bg_weight = background.a * (255 - alpha) / norm;

	return {
		static_cast<std::uint8_t>((foreground.r * fg_weight + background.r * bg_weight) / 255.0),
		static_cast<std::uint8_t>((foreground.g * fg_weight + background.g * bg_weight) / 255.0),
		static_cast<std::uint8_t>((foreground.b * fg_weight + background.b * bg_weight) / 255.0),
		static_cast<std::uint8_t>(norm)
	};
}

}  // namepsace gfx