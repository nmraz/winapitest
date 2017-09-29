#include "command_line.h"

#include "base/unicode.h"
#include <iomanip>
#include <memory>
#include <sstream>
#include <Windows.h>

using namespace std::literals;

namespace {

constexpr auto switch_sep = "--"sv;
constexpr auto switch_val_delim = "="sv;

std::string quote(const std::string& str) {
  if (str.find_first_of(" \t\"") == std::string::npos) {  // no quoting necessary
    return str;
  }

  std::string out = "\"";
  for (auto it = str.begin(); it != str.end(); ++it) {
    if (*it == '\\') {
      auto begin = it, end = it + 1;
      while (end != str.end() && *end == '\\') {
        ++end;
      }
      std::size_t backslashes = end - begin;

      // backslashes followed by a quote must be escaped
      if (*end == '"' || end == str.end()) {
        backslashes *= 2;
      }

      out.append(backslashes, '\\');
      it = end - 1;  // skip processed characters
    } else if (*it == '"') {
      out += "\\\"";
    } else {
      out += *it;
    }
  }
  out += '"';

  return out;
}

}  // namespace

namespace base {

command_line::command_line(int argc, const wchar_t* const* argv) {
  parse(argc, argv);
}

command_line::command_line(const wchar_t* cmd_line) {
  int argc;
  std::unique_ptr<wchar_t*, decltype(::LocalFree)*> argv(nullptr, ::LocalFree);

  argv.reset(::CommandLineToArgvW(cmd_line, &argc));
  parse(argc, argv.get());
}


std::optional<std::string> command_line::get_switch(std::string_view name) const {
  auto it = switches_.find(name);

  if (it == switches_.end()) {
    return std::nullopt;
  }
  return it->second;
}

bool command_line::has_flag(std::string_view name) const {
  auto sw = get_switch(name);
  return sw && *sw != "0";
}


std::string command_line::to_string() const {
  std::string ret = program_;

  for (const auto& sw : switches_) {
    ret += ' ' + quote(switch_sep.data() + sw.first);
    if (sw.second.size()) {
      ret += switch_val_delim.data() + quote(sw.second);
    }
  }

  if (args_.size()) {
    ret += " "s + switch_sep.data();
  }
  for (const auto& arg : args_) {
    ret += ' ' + quote(arg);
  }

  return ret;
}


void command_line::set_switch(std::string name, std::string value) {
  switches_[std::move(name)] = std::move(value);
}

void command_line::append_arg(std::string arg) {
  args_.push_back(std::move(arg));
}

void command_line::set_program(std::string program) {
  program_ = std::move(program);
}


void command_line::parse(int argc, const wchar_t* const* argv) {
  bool allow_switches = true;

  program_ = narrow(argv[0]);
  for (int i = 1; i < argc; i++) {
    std::string arg = narrow(argv[i]);

    allow_switches &= arg != switch_sep;

    if (arg.find(switch_sep) == 0 && allow_switches) {
      arg = arg.substr(switch_sep.size());
      std::size_t delim_pos = arg.find(switch_val_delim);

      if (delim_pos == std::string::npos) {
        set_switch(std::move(arg), "");
      } else {
        set_switch(arg.substr(0, delim_pos), arg.substr(delim_pos + switch_val_delim.size()));
      }
    } else {
      append_arg(std::move(arg));
    }
  }
}

}  // namepsace base