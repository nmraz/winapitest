#include "logging.h"

#include "base/thread/thread_name.h"
#include <mutex>
#include <thread>
#include <type_traits>

namespace {

std::mutex logging_lock;

std::unique_ptr<logging::sink> logging_sink;
logging::level min_logging_level;
bool colorize_logging;

const char* level_to_string(logging::level log_level) {
  using level_type = std::underlying_type_t<logging::level>;

  // maps [level log_level][bool color] to string
  // note: this depends on the order of the values defined in logging::level
  static constexpr const char* table[][2] = {
    {"TRACE", "\x1b[36mTRACE\x1b[0m"},
    {"INFO" , "\x1b[34mINFO\x1b[0m" },
    {"WARN" , "\x1b[33mWARN\x1b[0m" },
    {"ERROR", "\x1b[31mERROR\x1b[0m"}
  };

  return table[static_cast<level_type>(log_level)][colorize_logging];
}

}  // namespace

namespace logging {
namespace impl {

bool filter_out(level log_level) {
  using level_type = std::underlying_type_t<level>;

  return !logging_sink || static_cast<level_type>(log_level) < static_cast<level_type>(min_logging_level);
}


message::message(level log_level, const char* file, int line) {
  stream_ << "[" << level_to_string(log_level) << "][thread " << base::get_current_thread_name() << "]["
    << file << ":" << line << "] ";
}

message::~message() {
  stream_ << '\n';
  {
    std::scoped_lock hold(logging_lock);
    logging_sink->write(stream_.str().c_str());
  }
}

}  // namespace impl

void init(std::unique_ptr<sink> sink, level min_level, bool colorize) {
  logging_sink = std::move(sink);
  min_logging_level = min_level;
  colorize_logging = colorize;
}

}  // namespace logging