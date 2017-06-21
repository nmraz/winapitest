#pragma once

#include <memory>

namespace base {

namespace impl {
struct slot_rep_base;
}

class slot_handle {
public:
	slot_handle() = default;

	void disconnect();
	void block(bool block = true);
	bool blocked() const;
	bool connected() const { return !slot_.expired(); }

private:
	template<typename... Args>
	friend class signal;

	explicit slot_handle(std::weak_ptr<impl::slot_rep_base> slot)
		: slot_(std::move(slot)) {
	}

	std::weak_ptr<impl::slot_rep_base> slot_;
};

}  // namespace base