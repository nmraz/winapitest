#pragma once

#include "base/function.h"
#include "base/non_copyable.h"
#include <optional>
#include <Windows.h>

namespace base::win {

class message_window : public non_copy_movable {
public:
  using wnd_proc = function<std::optional<LRESULT>(UINT, WPARAM, LPARAM)>;

  message_window(wnd_proc proc);
  ~message_window();

  HWND get() { return hwnd_; }

private:
  struct class_registrar;

  static LRESULT CALLBACK main_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

  HWND hwnd_;
  wnd_proc wnd_proc_;
};

}  // namespace base::win