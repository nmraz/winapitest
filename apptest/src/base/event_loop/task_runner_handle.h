#pragma once

#include "base/event_loop/task.h"
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace base {

struct bad_task_runner_handle : std::runtime_error {
	bad_task_runner_handle();
};


class task_runner;

namespace impl {
struct task_runner_ref;
}

class task_runner_handle {
	friend task_runner;

public:
	task_runner_handle() = default;

	void post_task(task::callback_type callback);

	template<typename Cb>
	void post_task_with_caller(Cb&& callback);

	template<typename Cb, typename Then>
	void post_task_and_then(Cb&& callback, Then&& then) {
		do_post_task_and_then(std::forward<Cb>(callback), std::forward<Then>(then),
			              std::is_void<decltype(callback())>{});
	}

private:
	task_runner_handle(std::shared_ptr<impl::task_runner_ref> ref);
	static task_runner_handle current_hande();

	template<typename Cb, typename Then>
	void do_post_task_and_then(Cb&& callback, Then&& then, std::true_type);  // void return type

	template<typename Cb, typename Then>
	void do_post_task_and_then(Cb&& callback, Then&& then, std::false_type);  // non-void return type

	std::shared_ptr<impl::task_runner_ref> ref_;
};


template<typename Cb>
void task_runner_handle::post_task_with_caller(Cb&& callback) {
	post_task([caller = current_hande(), callback = std::forward<Cb>(callback)]() mutable {
		callback(std::move(caller));
	});
}


template<typename Cb, typename Then>
void task_runner_handle::do_post_task_and_then(Cb&& callback, Then&& then, std::true_type) {
	post_task_with_caller([callback = std::forward<Cb>(callback), then = std::forward<Then>(then)](task_runner_handle caller) {
		callback();
		caller.post_task(std::forward<Then>(then));
	});
}

template<typename Cb, typename Then>
void task_runner_handle::do_post_task_and_then(Cb&& callback, Then&& then, std::false_type) {
	post_task_with_caller([callback = std::forward<Cb>(callback), then = std::forward<Then>(then)](task_runner_handle caller) {
		caller.post_task([then = std::forward<Then>(then), tmp = callback()] {
			then(std::move(tmp));
		});
	});
}

}  // namespace base