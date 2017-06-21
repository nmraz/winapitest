#pragma once

#include "base/logging/logging.h"
#include <fstream>
#include <string_view>

namespace logging {

class file_sink : public sink {
public:
	explicit file_sink(std::string_view name);

	void write(const char* str) override;

private:
	std::ofstream file_;
};


class stdout_sink : public sink {
public:
	void write(const char* str) override;
};


class debugger_sink : public sink {
public:
	void write(const char* str) override;
};

}  // namespace logging