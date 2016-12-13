#include "logging.h"

#include <mutex>
#include <type_traits>

namespace {

const char* loggingLevelToString(logging::Level level, bool useColorCodes) {
	using LevelType = std::underlying_type_t<logging::Level>;

	// maps [Level level][bool color] to string
	// note: this depends on the order of the values defined in logging::Level
	constexpr const char* table[][2] = {
		{"TRACE", "\x1b[36mTRACE\x1b[0m"},
		{"INFO" , "\x1b[34mINFO\x1b[0m" },
		{"WARN" , "\x1b[33mWARN\x1b[0m" },
		{"ERROR", "\x1b[31mERROR\x1b[0m"}
	};

	return table[static_cast<LevelType>(level)][useColorCodes];
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