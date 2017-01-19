#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace base {

class CmdLine {
public:
	using Switches = std::map<std::string, std::string, std::less<>>;
	using Args = std::vector<std::string>;

	CmdLine() = default;
	CmdLine(int argc, const wchar_t* const* argv);
	explicit CmdLine(const wchar_t* cmdLine);

	std::optional<std::string> getSwitch(std::string_view name) const;
	bool hasFlag(std::string_view name) const;

	const Args& getArgs() const { return mArgs; }
	const Switches& getSwitches() const { return mSwitches; }

	std::string getProgram() const { return mProgram; }
	std::string getCmdLineString() const;

	void setSwitch(std::string name, std::string value = "");
	void appendArg(std::string arg);
	void setProgram(std::string program);

private:
	void parse(int argc, const wchar_t* const* argv);

	std::string mProgram;
	Switches mSwitches;
	Args mArgs;
};

}  // namespace base