#pragma once

#include "ui/gfx/base/util.h"
#include <cstdint>

namespace gfx {

struct color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
	std::uint8_t a = 0xff;
};


constexpr bool operator==(const color& rhs, const color& lhs) {
	return rhs.r == lhs.r && rhs.g == lhs.g && rhs.b == lhs.b && rhs.a == lhs.a;
}

constexpr bool operator!=(const color& rhs, const color& lhs) {
	return !(rhs == lhs);
}


namespace color_constants {

constexpr color transparent{0x00, 0x00, 0x00, 0x00};
constexpr color white{0xff, 0xff, 0xff};
constexpr color black{};

constexpr color red{0xff};
constexpr color green{0x00, 0xff};
constexpr color blue{0x00, 0x00, 0xff};

constexpr color yellow{0xff, 0xff};
constexpr color purple{0xff, 0x00, 0xff};
constexpr color cyan{0x00, 0xff, 0xff};

constexpr color orange{0xff, 0xa5};

}  // namespace color_constants


color lerp(color from, color to, double t);
color alpha_blend(color foreground, color background, std::uint8_t alpha);

}  // namespace gfx