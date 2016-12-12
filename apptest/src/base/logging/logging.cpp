#include "logging.h"

#include <mutex>
#include <type_traits>

namespace {

const char* loggingLevelToString(logging::Level level, bool useColorCodes) {
	using logging::Level;

	// TODO: find a cleaner way to implement this
	if (useColorCodes) {
		switch (level) {
		case Level::trace:
			return "\x1b[36mTRACE\x1b[0m";
		case Level::info:
			return "\x1b[34mINFO\x1b[0m";
		case Level::warn:
			return "\x1b[33mWARN\x1b[0m";
		case Level::error:
			return "\x1b[31mERROR\x1b[0m";
		default:
			return "UNKNOWN";
		}
	} else {
		switch (level) {
		case Level::trace:
			return "TRACE";
		case Level::info:
			return "INFO";
		case Level::warn:
			return "WARN";
		case Level::error:
			return "ERROR";
		default:
			return "UNKNOWN";
		}
	}
}

std::mutex gLoggingLock;

logging::Level gMinLoggingLevel;
std::unique_ptr<logging::Sink> gLoggingSink;

}  // namespace

namespace logging {
namespace impl {

bool filterOut(Level level) {
	using LevelType = std::underlying_type_t<Level>;

	return !gLoggingSink || static_cast<LevelType>(level) < static_cast<LevelType>(gMinLoggingLevel);
}


Message::Message(Level level, const char* file, int line) {
	mStream << "[" << loggingLevelToString(level, gLoggingSink->shouldUseColor()) << "][" << file << "(" << line << ")] ";
}

Message::~Message() {
	mStream << '\n';
	{
		std::lock_guard<std::mutex> hold(gLoggingLock);
		gLoggingSink->write(mStream.str().c_str());
	}
}

}  // namespace impl

void init(std::unique_ptr<Sink> sink, Level minLevel) {
	gLoggingSink = std::move(sink);
	gMinLoggingLevel = minLevel;
}

}  // namespace logging