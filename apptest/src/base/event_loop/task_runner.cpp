#include "task_runner.h"

#include "base/assert.h"
#include "base/event_loop/event_loop.h"
#include "base/event_loop/task_runner_handle.h"
#include "base/event_loop/task_runner_ref.h"
#include <utility>

namespace base {

task_runner::task_runner()
	: current_loop_(nullptr)
	, handle_ref_(std::make_shared<impl::task_runner_ref>(this)) {
}

task_runner::~task_runner() {
	std::lock_guard<std::shared_mutex> hold(handle_ref_->lock);
	handle_ref_->runner = nullptr;
}


void task_runner::post_task(task::callback_type callback, const task::delay_type& delay) {
	ASSERT(delay.count() >= 0) << "Can't post a task with a negative delay";

	task::run_time_type run_time = delay.count() == 0 ? task::run_time_type() : task::clock_type::now() + delay;
	bool was_empty;
	{
		std::lock_guard<std::mutex> hold(task_lock_);

		was_empty = task_queue_.empty();
		task_queue_.emplace(std::move(callback), run_time);
	}

	if (was_empty) {
		std::lock_guard<std::mutex> hold_loop(loop_lock_);  // current_loop_ mustn't change until after wake_up
		if (current_loop_) {
			current_loop_->wake_up();
		}
	}
}

void task_runner::post_quit() {
	post_task([this] { quit_now(); });
}

void task_runner::quit_now() {
	if (event_loop::is_nested()) {
		post_quit();
	}

	event_loop::current().quit();
}


task_runner_handle task_runner::handle() {
	return task_runner_handle(handle_ref_);
}


// static
task_runner& task_runner::current() {
	static thread_local task_runner runner;
	return runner;
}


// PRIVATE

bool task_runner::run_pending_task() {
	while (true) {
		swap_queues();
		if (current_tasks_.empty()) {
			break;  // no more tasks, not even in the incoming queue
		}

		while (!current_tasks_.empty()) {
			task current_task = std::move(current_tasks_.front());
			current_tasks_.pop();

			if (current_task.run_time == task::run_time_type()) {
				current_task.callback();
				return true;
			}
			delayed_tasks_.push(std::move(current_task));
		}
	}

	return false;
}

bool task_runner::run_delayed_task() {
	if (delayed_tasks_.empty()) {
		return false;
	}

	const task& current_task = delayed_tasks_.top();

	if (current_task.run_time > cached_now_) {
		if (current_task.run_time > (cached_now_ = task::clock_type::now())) {
			return false;
		}
	}

	task tmp_task = std::move(const_cast<task&>(current_task));  // current_task will still be at the top
	delayed_tasks_.pop();  // the task must be dequeued *before* running
	tmp_task.callback();

	return true;
}


std::optional<task::run_time_type> task_runner::get_next_run_time() const {
	if (delayed_tasks_.empty()) {
		return std::nullopt;
	}

	return delayed_tasks_.top().run_time;
}


void task_runner::swap_queues() {
	if (current_tasks_.empty()) {
		std::lock_guard<std::mutex> hold(task_lock_);
		task_queue_.swap(current_tasks_);
	}
}


void task_runner::set_loop(event_loop* loop) {
	std::lock_guard<std::mutex> hold(loop_lock_);
	current_loop_ = loop;
}

}  // namespace base