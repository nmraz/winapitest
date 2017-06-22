#pragma once

#include "base/event_loop/task.h"
#include "base/non_copyable.h"
#include <optional>

namespace base {

class task_runner;

class event_loop : public non_copy_movable {
public:
	void run();
	void quit();

	virtual ~event_loop() = default;

	virtual bool do_work();
	virtual void sleep(const std::optional<task::delay_type>& delay) = 0;
	virtual void wake_up() = 0;

	static event_loop& current();
	static bool is_nested();

protected:
	bool run_pending_task();
	bool run_delayed_task();
	std::optional<task::delay_type> next_delay();

private:
	struct loop_pusher;

	// Check that the loop is active and retrieve the current task_runner
	task_runner* get_runner();

	bool should_quit_;
	task_runner* runner_ = nullptr;
};

}  // namespace base
