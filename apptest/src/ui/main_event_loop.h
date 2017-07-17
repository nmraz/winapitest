#pragma once

#include "ui/event_loop_base.h"

namespace ui {

class main_event_loop : public event_loop_base {
public:
	void process_message(const MSG& msg) override;
};

}  // namepsace ui