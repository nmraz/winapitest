#pragma once

#include "base/NonCopyable.h"
#include "base/eventLoop/Task.h"
#include "base/eventLoop/TaskRunnerHandle.h"
#include <memory>
#include <mutex>
#include <optional>
#include <queue>

namespace base {

class event_loop;

namespace impl {
struct task_runner_ref;
}


class task_runner : public non_copy_movable {
	friend event_loop;

	task_runner();
	~task_runner();

public:
	void post_task(task::callback_type callback, const task::delay_type& delay = task::delay_type::zero());
	void quit_now();
	void post_quit();

	task_runner_handle handle();

	static task_runner& current();

private:
	using task_queue = std::queue<task>;
	using delayed_task_queue = std::priority_queue<task>;

	void set_loop(event_loop* loop);

	bool run_pending_task();
	bool run_delayed_task();
	std::optional<task::delay_type> next_delay() const;

	task_queue task_queue_;
	std::mutex task_lock_;  // protects task_queue_, current_loop_

	task_queue current_tasks_;  // to avoid locking the mutex every time, process tasks in batches
	delayed_task_queue delayed_tasks_;

	task::run_time_type cached_now_;  // make running more efficient when multiple tasks have to run now

	event_loop* current_loop_;

	std::shared_ptr<impl::task_runner_ref> handle_ref_;  // synchronizes with handles on other threads
};

}  // namespace base