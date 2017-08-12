#pragma once

#include "base/event_loop/task.h"

namespace base {
namespace impl {

void do_set_timeout(task::callback_type&& callback, const task::delay_type& delay);

}  // namespace impl


inline void next_tick(task::callback_type callback) {
	impl::do_set_timeout(std::move(callback), task::delay_type::zero());
}

template<typename Rep, typename Period>
inline void set_timeout(task::callback_type callback, const std::chrono::duration<Rep, Period>& delay) {
	impl::do_set_timeout(std::move(callback), std::chrono::ceil<task::delay_type>(delay));
}

}  // namespace base