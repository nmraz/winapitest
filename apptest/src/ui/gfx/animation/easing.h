#pragma once

namespace gfx {
namespace easing {
namespace impl {

double cubic_bezier(double a, double b, double c, double d, double p);

}  // namespace impl

double linear(double p);
double ease(double p);
double ease_in(double p);
double ease_out(double p);
double ease_in_out(double p);

auto make_cubic_bezier(double a, double b, double c, double d) {
	return [=](double p) { 
		return impl::cubic_bezier(a, b, c, d, p); 
	};
}

}  // namespace easing
}  // namespace gfx