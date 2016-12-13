#pragma once

#include <memory>
#include <sstream>

#define LOG_FULL(level, file, line) ::logging::impl::filterOut(logging::Level::level) ? (void) 0 : \
	::logging::impl::StreamVoidify() | ::logging::impl::Message(logging::Level::level, file, line).stream()

#define LOG(level) LOG_FULL(level, __FILE__, __LINE__)


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


struct StreamVoidify {
	void operator|(std::ostream&) {}
};


class Message {
public:
	Message(Level level, const char* file, int line);
	~Message();

	std::ostream& stream() { return mStream; }

private:
	std::ostringstream mStream;
};

}  // namespace impl

struct Sink {
	virtual ~Sink() = default;

	virtual void write(const char* str) = 0;
};

void init(std::unique_ptr<Sink> sink, Level minLevel, bool colorize);

}  // namespace logging