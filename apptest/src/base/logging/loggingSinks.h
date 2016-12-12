#pragma once

#include "base/logging/logging.h"
#include <fstream>
#include <string_view>

namespace logging {

class FileSink : public Sink {
public:
	explicit FileSink(std::string_view name);

	void write(const char* str) override;

private:
	std::ofstream mFile;
};


class StdoutSink : public Sink {
public:
	void write(const char* str) override;
	bool shouldUseColor() const override { return true; }
};


class DebuggerSink : public Sink {
public:
	void write(const char* str) override;
};

}  // namespace logging