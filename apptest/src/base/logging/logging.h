#pragma once

#include <memory>
#include <sstream>

#define LOG_FULL(LEVEL, FILE, LINE) ::logging::impl::filterOut(logging::Level::LEVEL) ? (void) 0 : \
	::logging::impl::MsgVoidify() | ::logging::impl::Message(logging::Level::LEVEL, FILE, LINE)

#define LOG(LEVEL) LOG_FULL(LEVEL, __FILE__, __LINE__)


namespace logging {

// logging.cpp depends on this order!
enum class Level {
	trace = 0,
	info,
	warn,
	error
};

namespace impl {

bool filterOut(Level level);


class Message {
public:
	Message(Level level, const char* file, int line);
	~Message();

	template<typename T>
	Message& operator<<(T&& val) {
		mStream << std::forward<T>(val);
		return *this;
	}

private:
	std::ostringstream mStream;
};


struct MsgVoidify {
	void operator|(const Message&) {}
};

}  // namespace impl

struct Sink {
	virtual ~Sink() = default;

	virtual void write(const char* str) = 0;
};

void init(std::unique_ptr<Sink> sink, Level minLevel, bool colorize);

}  // namespace logging