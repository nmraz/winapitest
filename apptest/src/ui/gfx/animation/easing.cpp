#include "easing.h"

namespace gfx {
namespace easing {
namespace impl {

double cubic_bezier(double a, double b, double c, double d, double p) {
	double p_sqr = p * p;
	double p_inv = 1 - p;
	double p_inv_sqr = p_inv * p_inv;

	return a * p * p_sqr + b * p_sqr * p_inv + c * p * p_inv_sqr + d * p_inv * p_inv_sqr;
}

}  // namespace impl

// values from css spec
double linear(double p) {
	return p;
}
double ease(double p) {
	return impl::cubic_bezier(0.25, 0.1, 0.25, 1, p);
}
double ease_in(double p) {
	return impl::cubic_bezier(0.42, 0, 1, 1, p);
}
double ease_out(double p) {
	return impl::cubic_bezier(0, 0, 0.58, 1, p);
}
double ease_in_out(double p) {
	return impl::cubic_bezier(0.42, 0, 0.58, 1, p);
}


}  // namespace easing
}  // namespace gfx