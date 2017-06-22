#include "event_loop.h"

#include "base/assert.h"
#include "base/auto_restore.h"
#include "base/event_loop/task_runner.h"

namespace base {
namespace {

thread_local event_loop* current_loop = nullptr;
thread_local int nesting_level = 0;

}  // namespace


struct event_loop::loop_pusher {
	loop_pusher(event_loop* loop);
	~loop_pusher();

	void set_loop(event_loop* loop);

	event_loop* prev_loop_;
};

event_loop::loop_pusher::loop_pusher(event_loop* loop)
	: prev_loop_(current_loop) {
	set_loop(loop);
}

event_loop::loop_pusher::~loop_pusher() {
	set_loop(prev_loop_);
}

void event_loop::loop_pusher::set_loop(event_loop* loop) {
	current_loop = loop;
	task_runner::current().set_loop(loop);
}


void event_loop::run() {
	loop_pusher push(this);
	auto_restore<int> restore_nesting(nesting_level);
	auto_restore<task_runner*> restore_runner(runner_, &task_runner::current());

	++nesting_level;
	should_quit_ = false;

	while (true) {
		bool ran_task = run_pending_task();
		if (should_quit_) {
			break;
		}

		ran_task |= run_delayed_task();
		if (should_quit_) {
			break;
		}

		ran_task |= do_work();
		if (should_quit_) {
			break;
		}

		if (!ran_task) {
			sleep(next_delay());
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
	return get_runner()->run_pending_task();
}

bool event_loop::run_delayed_task() {
	return get_runner()->run_delayed_task();
}

std::optional<task::delay_type> event_loop::next_delay() {
	return get_runner()->next_delay();
}


// PRIVATE

task_runner* event_loop::get_runner() {
	ASSERT(runner_ && this == current_loop) << "Only the active event_loop can run tasks";
	return runner_;
}

}  // namespace base