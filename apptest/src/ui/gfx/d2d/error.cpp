#include "error.h"
#include <sstream>
#include <string>

namespace gfx::impl {
namespace {

class d2d_error_category : public std::error_category {
	const char* name() const noexcept override;
	std::string message(int code) const noexcept override;
};


const char* d2d_error_category::name() const noexcept {
	return "D2D";
}

std::string d2d_error_category::message(int code) const noexcept {
	switch (static_cast<d2d_errc>(code)) {
	case d2d_errc::out_of_memory:
		return "out of memory";
	case d2d_errc::invalid_arg:
		return "invalid argument";
	case d2d_errc::bad_number:
		return "bad number";
	case d2d_errc::bitmap_bound_as_target:
		return "invalid target bitmap";
	case d2d_errc::bitmap_cannot_draw:
		return "cannot draw bitmap";
	case d2d_errc::cyclic_graph:
		return "illegal cycle in graph";
	case d2d_errc::unsupported_display_format:
		return "unsupported display format";
	case d2d_errc::invalid_display_state:
		return "invalid display state";
	case d2d_errc::unregistered_effect:
		return "effect not registered";
	case d2d_errc::oversized_bitmap:
		return "oversized bitmap";
	case d2d_errc::oversized_texture:
		return "oversized texture";
	case d2d_errc::incompatible_brush_types:
		return "incompatible brush types";
	case d2d_errc::insufficient_buffer:
		return "buffer too small";
	case d2d_errc::insufficient_device_caps:
		return "insufficient device capabilities";
	case d2d_errc::intermediate_too_large:
		return "intermediate too large";
	case d2d_errc::internal_err:
		return "internal direct2d error";
	case d2d_errc::invalid_call:
		return "invalid call";
	case d2d_errc::invalid_property:
		return "invalid property";
	case d2d_errc::invalid_target:
		return "invalid target";
	case d2d_errc::invalid_graph_config:
		return "invalid graph configuration";
	case d2d_errc::invalid_intern_graph_config:
		return "invalid internal graph configuration";
	case d2d_errc::layer_in_use:
		return "layer already in use";
	case d2d_errc::no_hardware_device:
		return "no hardware device available";
	case d2d_errc::no_sub_props:
		return "the specified sub-property does not exist";
	case d2d_errc::not_initialized:
		return "the object is not initialized";
	case d2d_errc::original_target_not_bound:
		return "original target not bound";
	case d2d_errc::outstanding_bitmap_refs:
		return "outstanding bitmap references";
	case d2d_errc::invalid_pop:
		return "invalid pop call";
	case d2d_errc::unbalanced_push_pop:
		return "unbalanced calls to push and pop";
	case d2d_errc::unsupported_print_fmt:
		return "unsupported print format";
	case d2d_errc::print_job_closed:
		return "print job closed";
	case d2d_errc::recreate_target:
		return "the render target must be recreated";
	case d2d_errc::unexpected_clip:
		return  "cannot copy from clipped layer";
	case d2d_errc::scanner_failed:
		return "geometry scanner failed";
	case d2d_errc::shader_compilation_failed:
		return "shader compilation failed";
	case d2d_errc::screen_access_denied:
		return "screen access denied";
	case d2d_errc::target_not_gdi_compatible:
		return "target not compatible with GDI";
	case d2d_errc::wrong_text_effect_type:
		return "wrong text effect type";
	case d2d_errc::unreleased_text_renderer:
		return "text renderer not released";
	case d2d_errc::too_many_shader_elements:
		return "too many shader elements";
	case d2d_errc::too_many_transform_inputs:
		return "too many transform inputs";
	case d2d_errc::unsupported_operation:
		return "unsupported operation";
	case d2d_errc::unsupported_pixel_format:
		return "unsupported pixel format";
	case d2d_errc::unsupported_version:
		return "unsupported version";
	case d2d_errc::unknown_win32_err:
		return "unknown windows error";
	case d2d_errc::wrong_factory:
		return "wrong factory";
	case d2d_errc::wrong_resource_domain:
		return "wrong resource domain";
	case d2d_errc::wrong_state:
		return "wrong state";
	case d2d_errc::zero_vector:
		return "zero vector";
	default:
		std::ostringstream stream;
		stream << "direct2d error " << std::hex << code;
		return stream.str();
	}
}

}  // namepsace


std::error_code make_error_code(d2d_errc err) {
	return std::error_code(static_cast<int>(err), d2d_category());
}

std::error_category& d2d_category() {
	static d2d_error_category cat;
	return cat;
}

void throw_if_failed(HRESULT hr, const char* what) {
	if (!SUCCEEDED(hr)) {
		throw std::system_error(static_cast<d2d_errc>(hr), what);
	}
}

}  // namepsace gfx::impl