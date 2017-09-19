#pragma once

#include "base/event_loop/event_loop.h"
#include "base/win/scoped_handle.h"

namespace base {

class io_event_loop : public event_loop {
public:
	io_event_loop();

	void sleep(const task::delay_type* delay) override;
	void wake_up() override;

private:
	win::scoped_handle wake_up_evt_;
};

}  // namespace base