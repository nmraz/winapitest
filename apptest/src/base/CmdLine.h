#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace base {

class CmdLine {
public:
	CmdLine() = default;
	CmdLine(int argc, const wchar_t* const* argv);
	explicit CmdLine(const wchar_t* cmdLine);

	std::optional<std::string> getSwitch(std::string_view name) const;
	const std::vector<std::string>& getArgs() const { return mArgs; }
	std::string_view getProgram() const { return mProgram; }
	
	bool hasFlag(std::string_view name) const;

	std::string getCmdLineString() const;

	void setSwitch(std::string name, std::string value = "");
	void appendArg(std::string arg);
	void setProgram(std::string program);

private:
	void parse(int argc, const wchar_t* const* argv);

	std::string mProgram;

	std::map<std::string, std::string, std::less<>> mSwitches;
	std::vector<std::string> mArgs;
};

}  // namespace base