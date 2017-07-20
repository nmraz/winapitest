#pragma once

#include <system_error>
#include <d2d1.h>

namespace gfx::impl {

enum class d2d_errc : HRESULT {
	out_of_memory = E_OUTOFMEMORY,
	invalid_arg = E_INVALIDARG,

	bad_number = D2DERR_BAD_NUMBER,
	bitmap_bound_as_target = D2DERR_BITMAP_BOUND_AS_TARGET,
	bitmap_cannot_draw = D2DERR_BITMAP_CANNOT_DRAW,
	cyclic_graph = D2DERR_CYCLIC_GRAPH,
	unsupported_display_format = D2DERR_DISPLAY_FORMAT_NOT_SUPPORTED,
	invalid_display_state = D2DERR_DISPLAY_STATE_INVALID,
	unregistered_effect = D2DERR_EFFECT_IS_NOT_REGISTERED,
	oversized_bitmap = D2DERR_EXCEEDS_MAX_BITMAP_SIZE,
	oversized_texture = D2DERR_MAX_TEXTURE_SIZE_EXCEEDED,
	incompatible_brush_types = D2DERR_INCOMPATIBLE_BRUSH_TYPES,
	insufficient_buffer = D2DERR_INSUFFICIENT_BUFFER,
	insufficient_device_caps = D2DERR_INSUFFICIENT_DEVICE_CAPABILITIES,
	intermediate_too_large = D2DERR_INTERMEDIATE_TOO_LARGE,
	internal_err = D2DERR_INTERNAL_ERROR,
	invalid_call = D2DERR_INVALID_CALL,
	invalid_property = D2DERR_INVALID_PROPERTY,
	invalid_target = D2DERR_INVALID_TARGET,
	invalid_graph_config = D2DERR_INVALID_GRAPH_CONFIGURATION,
	invalid_intern_graph_config = D2DERR_INVALID_INTERNAL_GRAPH_CONFIGURATION,
	layer_in_use = D2DERR_LAYER_ALREADY_IN_USE,
	no_hardware_device = D2DERR_NO_HARDWARE_DEVICE,
	no_sub_props = D2DERR_NO_SUBPROPERTIES,
	not_initialized = D2DERR_NOT_INITIALIZED,
	original_target_not_bound = D2DERR_ORIGINAL_TARGET_NOT_BOUND,
	outstanding_bitmap_refs = D2DERR_OUTSTANDING_BITMAP_REFERENCES,
	invalid_pop = D2DERR_POP_CALL_DID_NOT_MATCH_PUSH,
	unbalanced_push_pop = D2DERR_PUSH_POP_UNBALANCED,
	unsupported_print_fmt = D2DERR_PRINT_FORMAT_NOT_SUPPORTED,
	print_job_closed = D2DERR_PRINT_JOB_CLOSED,
	recreate_target = D2DERR_RECREATE_TARGET,
	unexpected_clip = D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT,
	scanner_failed = D2DERR_SCANNER_FAILED,
	shader_compilation_failed = D2DERR_SHADER_COMPILE_FAILED,
	screen_access_denied = D2DERR_SCREEN_ACCESS_DENIED,
	target_not_gdi_compatible = D2DERR_TARGET_NOT_GDI_COMPATIBLE,
	wrong_text_effect_type = D2DERR_TEXT_EFFECT_IS_WRONG_TYPE,
	unreleased_text_renderer = D2DERR_TEXT_RENDERER_NOT_RELEASED,
	too_many_shader_elements = D2DERR_TOO_MANY_SHADER_ELEMENTS,
	too_many_transform_inputs = D2DERR_TOO_MANY_TRANSFORM_INPUTS,
	unsupported_operation = D2DERR_UNSUPPORTED_OPERATION,
	unsupported_pixel_format = D2DERR_UNSUPPORTED_PIXEL_FORMAT,
	unsupported_version = D2DERR_UNSUPPORTED_VERSION,
	unknown_win32_err = D2DERR_WIN32_ERROR,
	wrong_factory = D2DERR_WRONG_FACTORY,
	wrong_resource_domain = D2DERR_WRONG_RESOURCE_DOMAIN,
	wrong_state = D2DERR_WRONG_STATE,
	zero_vector = D2DERR_ZERO_VECTOR
};

std::error_code make_error_code(d2d_errc err);
std::error_category& d2d_category();

void throw_if_failed(HRESULT hr, const char* what = "");

}  // namespace gfx::impl

template<>
struct std::is_error_code_enum<gfx::impl::d2d_errc> : std::true_type {
};