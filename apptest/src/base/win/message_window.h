#pragma once

#include "base/non_copyable.h"
#include <functional>
#include <Windows.h>

namespace base::win {

class message_window : public non_copy_movable {
public:
	using wnd_proc = std::function<LRESULT(UINT, WPARAM, LPARAM)>;

	message_window(wnd_proc proc);
	HWND get() { return hwnd_; }

private:
	static ATOM register_class();
	static LRESULT CALLBACK main_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

	HWND hwnd_;
	wnd_proc wnd_proc_;
};

}  // namespace base::win