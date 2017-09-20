#pragma once

#include "base/event_loop/task.h"
#include "base/non_copyable.h"
#include <chrono>
#include <functional>
#include <memory>

namespace base {

class timer : public non_copy_movable {
public:
	using callback_type = std::function<void()>;
	
	explicit timer(callback_type callback = nullptr);
	~timer();

	template<typename Rep, typename Period>
	void set(const std::chrono::duration<Rep, Period>& interval, bool repeat = false);
	void cancel();

	bool is_running() const;

	void set_callback(callback_type callback);

private:
	struct posted_task;

	void do_set(const task::delay_type& delay, bool repeat);
	void fire();
	void repost_task();

	bool repeating_;
	task::delay_type interval_;

	std::shared_ptr<posted_task> current_task_;
	callback_type callback_;
};

template<typename Rep, typename Period>
inline void timer::set(const std::chrono::duration<Rep, Period>& interval, bool repeat) {
	do_set(std::chrono::ceil<task::delay_type>(interval), repeat);
}

}  // namespace base
