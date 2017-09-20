#include "message_window.h"

#include "base/win/last_error.h"
#include <utility>


constexpr const wchar_t* wnd_class_name = L"BaseMessageWnd";

namespace base::win {

struct message_window::class_registrar {
	class_registrar();
	~class_registrar();
};

message_window::class_registrar::class_registrar() {
	WNDCLASSEXW wnd_class = {};
	wnd_class.cbSize = sizeof(WNDCLASSEXW);
	wnd_class.lpfnWndProc = &main_wnd_proc;
	wnd_class.hInstance = ::GetModuleHandleW(nullptr);
	wnd_class.lpszClassName = wnd_class_name;
	if (!::RegisterClassExW(&wnd_class)) {
		throw_last_error("Failed to register class");
	}
}

message_window::class_registrar::~class_registrar() {
	::UnregisterClassW(wnd_class_name, ::GetModuleHandleW(nullptr));
}


message_window::message_window(wnd_proc proc)
	: wnd_proc_(std::move(proc)) {
	static class_registrar registrar;
	hwnd_ = ::CreateWindowExW(0, wnd_class_name, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, ::GetModuleHandleW(nullptr), this);
}


// PRIVATE

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

}  // namespace base::win