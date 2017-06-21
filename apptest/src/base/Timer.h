#pragma once

#include "base/eventLoop/Task.h"
#include "base/NonCopyable.h"
#include "base/signal/Signal.h"
#include <chrono>
#include <memory>

namespace base {

class timer : public non_copy_movable {
	using fire_signal = signal<>;

public:
	~timer();

	template<typename Rep, typename Period>
	void set(const std::chrono::duration<Rep, Period>& interval, bool repeat = false);
	void cancel();

	bool is_running() const;

	slot_handle on_fire(fire_signal::slot_type slot);

private:
	struct posted_task;

	void do_set(const task::delay_type& delay, bool repeat);
	void fire();
	void repost_task();

	bool repeating_;
	task::delay_type interval_;

	std::shared_ptr<posted_task> current_task_;
	fire_signal fire_signal_;
};

template<typename Rep, typename Period>
inline void timer::set(const std::chrono::duration<Rep, Period>& interval, bool repeat) {
	do_set(std::chrono::ceil<task::delay_type>(interval), repeat);
}

}  // namespace base
