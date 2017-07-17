#include "main_event_loop.h"

namespace ui {

void main_event_loop::process_message(const MSG& msg) {
	::TranslateMessage(&msg);
	::DispatchMessageW(&msg);
}

}  // namespace ui