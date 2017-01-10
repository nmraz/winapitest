#include "CmdLine.h"

#include "base/unicode.h"
#include <iomanip>
#include <memory>
#include <sstream>
#include <Windows.h>

namespace {

constexpr const char* switchSep = "--";
constexpr std::size_t switchSepLen = 2;

constexpr const char* switchValDelim = "=";

std::string quote(const std::string& str) {
	std::ostringstream out;
	out << std::quoted(str);
	return out.str();
}

}  // namespace

namespace base {

CmdLine::CmdLine(int argc, const wchar_t* const* argv) {
	parse(argc, argv);
}

CmdLine::CmdLine(const wchar_t* cmdLine) {
	int argc;
	std::unique_ptr<wchar_t*, decltype(::LocalFree)*> argv(nullptr, ::LocalFree);

	argv.reset(::CommandLineToArgvW(cmdLine, &argc));
	parse(argc, argv.get());
}


std::optional<std::string> CmdLine::getSwitch(std::string_view name) const {
	auto it = mSwitches.find(name);

	if (it == mSwitches.end()) {
		return std::nullopt;
	}
	return it->second;
}

bool CmdLine::hasFlag(std::string_view name) const {
	auto sw = getSwitch(name);
	return sw && *sw != "0";
}


std::string CmdLine::getCmdLineString() const {
	std::string ret = mProgram + ' ';

	for (const auto& sw : mSwitches) {
		ret += switchSep + sw.first;
		if (sw.second.size()) {
			ret += switchValDelim + quote(sw.second);
		}
		ret += ' ';
	}
	ret += switchSep;
	for (const auto& arg : mArgs) {
		ret += ' ' + quote(arg);
	}

	return ret;
}


void CmdLine::setSwitch(std::string name, std::string value) {
	mSwitches[std::move(name)] = std::move(value);
}

void CmdLine::appendArg(std::string arg) {
	mArgs.push_back(std::move(arg));
}

void CmdLine::setProgram(std::string program) {
	mProgram = std::move(program);
}


void CmdLine::parse(int argc, const wchar_t* const* argv) {
	bool allowSwitches = true;

	for (int i = 0; i < argc; i++) {
		std::string arg = u16ToU8(argv[i]);

		allowSwitches &= arg != switchSep;

		if (arg.find(switchSep) == 0 && allowSwitches) {
			arg = arg.substr(switchSepLen);
			std::size_t delimPos = arg.find(switchValDelim);

			if (delimPos == std::string::npos) {
				setSwitch(std::move(arg), "");
			} else {
				setSwitch(arg.substr(0, delimPos), arg.substr(delimPos + 1));
			}
		} else {
			appendArg(std::move(arg));
		}
	}
}

}  // namepsace base