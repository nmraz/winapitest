#include "point.h"

#include "base/assert.h"
#include "ui/gfx/util.h"
#include <cmath>

namespace gfx {

float mag(const pointf& pt) {
	return std::sqrt(mag_squared(pt));
}

float angle(const pointf& pt) {
	float raw_angle = std::atan2(pt.y, pt.x);
	return raw_angle < 0.f ? raw_angle + two_pi<float> : raw_angle;
}

pointf to_unit(const pointf& pt) {
	ASSERT(mag_squared(pt) > 0) << "Cannot normalize vector of zero length";
	return pt / mag(pt);
}

pointf point_for_angle(float angle, float rx, float ry) {
	ASSERT(rx >= 0.f && ry >= 0.f) << "Radii must be non-negative";
	ASSERT(rx != 0.f || ry != 0.f) << "Cannot rotate vector of zero length";


	return { rx * std::cos(angle), ry * std::sin(angle) };
}

}  // namespace gfx