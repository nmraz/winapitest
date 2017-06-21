#include "EventLoop.h"

#include "base/assert.h"
#include "base/AutoRestore.h"
#include "base/eventLoop/TaskRunner.h"

namespace base {
namespace {

thread_local event_loop* current_loop = nullptr;
thread_local int nesting_level = 0;

}  // namespace


struct event_loop::loop_pusher {
	loop_pusher(event_loop* loop);
	~loop_pusher();

	event_loop* prev_loop_;
};

event_loop::loop_pusher::loop_pusher(event_loop* loop)
	: prev_loop_(current_loop) {
	current_loop = loop;
	task_runner::current().set_loop(loop);
}

event_loop::loop_pusher::~loop_pusher() {
	current_loop = prev_loop_;
	task_runner::current().set_loop(prev_loop_);
}


void event_loop::run() {
	loop_pusher push(this);
	auto_restore<int> restore_nesting(nesting_level);

	++nesting_level;
	should_quit_ = false;

	task_runner& runner = task_runner::current();

	while (true) {
		bool ran_task = runner.run_pending_task();
		if (should_quit_) {
			break;
		}

		ran_task |= runner.run_delayed_task();
		if (should_quit_) {
			break;
		}

		ran_task |= do_work();
		if (should_quit_) {
			break;
		}

		if (!ran_task) {
			sleep(runner.next_delay());
		}
	}
}

void event_loop::quit() {
	should_quit_ = true;
}


bool event_loop::do_work() {
	return false;
}


// static
event_loop& event_loop::current() {
	ASSERT(current_loop) << "No event loop running on this thread";
	return *current_loop;
}

// static
bool event_loop::is_nested() {
	return nesting_level > 1;
}


// PROTECTED

bool event_loop::run_pending_task() {
	return task_runner::current().run_pending_task();
}

bool event_loop::run_delayed_task() {
	return task_runner::current().run_delayed_task();
}

std::optional<task::delay_type> event_loop::next_delay() {
	return task_runner::current().next_delay();
}

}  // namespace base