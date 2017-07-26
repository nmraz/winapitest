#include "error.h"

#include "base/unicode.h"
#include <sstream>
#include <string>
#include <Windows.h>

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
	wchar_t* msg;
	DWORD msg_len = ::FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		nullptr,
		code,
		0,
		reinterpret_cast<wchar_t*>(&msg),
		0,
		nullptr
	);

	if (!msg_len) {
		return "unknown direct2d error";
	}
	std::string ret = base::narrow(msg);
	::LocalFree(msg);
	return ret;
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