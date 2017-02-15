#include "logging.h"

#include <mutex>
#include <thread>
#include <type_traits>

namespace {

const char* loggingLevelToString(logging::Level level, bool colorize) {
	using LevelType = std::underlying_type_t<logging::Level>;

	// maps [Level level][bool color] to string
	// note: this depends on the order of the values defined in logging::Level
	static constexpr const char* table[][2] = {
		{"TRACE", "\x1b[36mTRACE\x1b[0m"},
		{"INFO" , "\x1b[34mINFO\x1b[0m" },
		{"WARN" , "\x1b[33mWARN\x1b[0m" },
		{"ERROR", "\x1b[31mERROR\x1b[0m"}
	};

	return table[static_cast<LevelType>(level)][colorize];
}

std::mutex gLoggingLock;

std::unique_ptr<logging::Sink> gLoggingSink;
logging::Level gMinLoggingLevel;
bool gColorizeLogging;

}  // namespace

namespace logging {
namespace impl {

bool filterOut(Level level) {
	using LevelType = std::underlying_type_t<Level>;

	return !gLoggingSink || static_cast<LevelType>(level) < static_cast<LevelType>(gMinLoggingLevel);
}


Message::Message(Level level, const char* file, int line) {
	mStream << "[" << loggingLevelToString(level, gColorizeLogging) << "][thread " << std::this_thread::get_id() << "]["
		<< file << ":" << line << "] ";
}

Message::~Message() {
	mStream << '\n';
	{
		std::lock_guard<std::mutex> hold(gLoggingLock);
		gLoggingSink->write(mStream.str().c_str());
	}
}

}  // namespace impl

void init(std::unique_ptr<Sink> sink, Level minLevel, bool colorize) {
	gLoggingSink = std::move(sink);
	gMinLoggingLevel = minLevel;
	gColorizeLogging = colorize;
}

}  // namespace logging