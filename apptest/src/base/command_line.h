#pragma once

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace base {

class command_line {
public:
  using switches = std::map<std::string, std::string, std::less<>>;
  using args = std::vector<std::string>;

  command_line() = default;
  command_line(int argc, const wchar_t* const* argv);
  explicit command_line(const wchar_t* cmd_line);

  std::optional<std::string> get_switch(std::string_view name) const;
  bool has_flag(std::string_view name) const;

  const args& get_args() const { return args_; }
  const switches& get_switches() const { return switches_; }

  std::string get_program() const { return program_; }
  std::string to_string() const;

  void set_switch(std::string name, std::string value = "");
  void append_arg(std::string arg);
  void set_program(std::string program);

private:
  void parse(int argc, const wchar_t* const* argv);

  std::string program_;
  switches switches_;
  args args_;
};

}  // namespace base