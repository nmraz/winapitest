#pragma once

#include "base/eventLoop/Task.h"
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace base {

struct BadTaskRunnerHandle : std::runtime_error {
	BadTaskRunnerHandle();
};


class TaskRunner;

namespace impl {
struct TaskRunnerRef;
}

class TaskRunnerHandle {
	friend TaskRunner;

public:
	TaskRunnerHandle() = default;

	void postTask(Task::Callback callback);

	template<typename Cb, typename Then>
	void postTaskAndThen(Cb&& callback, Then&& then) {
		doPostTaskAndThen(std::forward<Cb>(callback), std::forward<Then>(then),
			              std::is_void<decltype(callback())>{});
	}

private:
	TaskRunnerHandle(std::shared_ptr<impl::TaskRunnerRef> ref);
	static TaskRunnerHandle currentHande();

	template<typename Cb, typename Then>
	void doPostTaskAndThen(Cb&& callback, Then&& then, std::true_type);  // void return type

	template<typename Cb, typename Then>
	void doPostTaskAndThen(Cb&& callback, Then&& then, std::false_type);  // non-void return type

	std::shared_ptr<impl::TaskRunnerRef> mRef;
};


template<typename Cb, typename Then>
void TaskRunnerHandle::doPostTaskAndThen(Cb&& callback, Then&& then, std::true_type) {
	postTask([callback = std::forward<Cb>(callback), then = std::forward<Then>(then),
		      caller = currentHande()]() mutable {
		callback();
		caller.postTask(std::forward<Then>(then));
	});
}

template<typename Cb, typename Then>
void TaskRunnerHandle::doPostTaskAndThen(Cb&& callback, Then&& then, std::false_type) {
	postTask([callback = std::forward<Cb>(callback), then = std::forward<Then>(then),
		      caller = currentHande()]() mutable {
		caller.postTask([then = std::forward<Then>(then), tmp = callback()] {
			then(std::move(tmp));
		});
	});
}

}  // namespace base