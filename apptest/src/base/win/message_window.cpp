#include "message_window.h"

#include "base/win/last_error.h"
#include <utility>

namespace base {
namespace win {

message_window::message_window(wnd_proc proc)
	: wnd_proc_(std::move(proc)) {
	ATOM wnd_class = register_class();
	hwnd_ = ::CreateWindowExW(0, MAKEINTATOM(wnd_class), nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, ::GetModuleHandleW(nullptr), this);
}


// PRIVATE

ATOM message_window::register_class() {
	static ATOM atom = 0;
	if (!atom) {
		WNDCLASSEXW wnd_class = {};
		wnd_class.cbSize = sizeof(WNDCLASSEXW);
		wnd_class.lpfnWndProc = &main_wnd_proc;
		wnd_class.hInstance = ::GetModuleHandleW(nullptr);
		wnd_class.lpszClassName = L"BASE_MESSAGE_WND";
		atom = ::RegisterClassExW(&wnd_class);
		if (!atom) {
			throw_last_error("Failed to register class");
		}
	}
	return atom;
}

LRESULT CALLBACK message_window::main_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
	message_window* target = nullptr;

	if (msg == WM_CREATE) {
		CREATESTRUCTW* create_struct = reinterpret_cast<CREATESTRUCTW*>(lparam);
		target = static_cast<message_window*>(create_struct->lpCreateParams);
		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(target));
	} else {
		target = reinterpret_cast<message_window*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	}

	if (target) {
		return target->wnd_proc_(msg, wparam, lparam);
	} else {
		return ::DefWindowProcW(hwnd, msg, wparam, lparam);
	}
}

}  // namespace win
}  // namepsace base