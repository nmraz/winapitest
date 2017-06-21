#pragma once

#include <memory>
#include <sstream>

#define LOG_FULL(LEVEL, FILE, LINE) ::logging::impl::filter_out(logging::level::LEVEL) ? (void) 0 : \
	::logging::impl::msg_voidify() | ::logging::impl::message(logging::level::LEVEL, FILE, LINE)

#define LOG(LEVEL) LOG_FULL(LEVEL, __FILE__, __LINE__)


namespace logging {

// logging.cpp depends on this order!
enum class level {
	trace = 0,
	info,
	warn,
	error
};

namespace impl {

bool filter_out(level level);


class message {
public:
	message(level level, const char* file, int line);
	~message();

	template<typename T>
	message& operator<<(T&& val) {
		stream_ << std::forward<T>(val);
		return *this;
	}

private:
	std::ostringstream stream_;
};


struct msg_voidify {
	void operator|(const message&) {}
};

}  // namespace impl

struct sink {
	virtual ~sink() = default;

	virtual void write(const char* str) = 0;
};

void init(std::unique_ptr<sink> sink, level min_level, bool colorize);

}  // namespace logging