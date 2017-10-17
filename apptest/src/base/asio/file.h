#pragma once

#include "base/future/future.h"
#include "base/win/scoped_handle.h"
#include <cstdint>
#include <functional>
#include <iterator>
#include <string_view>
#include <system_error>

namespace base {

class file {
public:
  enum access {
    in = 1 << 0,  // read
    out = 1 << 1  // write
  };

  enum class create_disp {
    open_only,  // open only if exists
    create_only,  // create only if doesn't exist

    open_always,  // open or create
    create_always,  // open or create + truncate
  };

  enum share_mode {
    share_none = 0,
    share_read = 1 << 0,
    share_write = 1 << 1,
    share_del = 1 << 2
  };


  using offset_type = std::int64_t;
  using complete_type = future<unsigned long>;

  file() = default;
  file(std::string_view name, int desired_access, create_disp disp, int share = share_read | share_write);

  void open(std::string_view name, int desired_access, create_disp disp, int share = share_read | share_write);
  void close();


  template<typename Buffer>
  complete_type read(offset_type offset, Buffer& buf);


  template<typename Buffer>
  complete_type write(offset_type offset, const Buffer& buf);

  template<typename Buffer>
  void write(offset_type, const Buffer&&) = delete;  // temp object


  complete_type read(offset_type offset, void* buf, unsigned long count);
  complete_type write(offset_type offset, const void* buf, unsigned long count);

  offset_type length();

private:
  win::scoped_handle handle_;
};


template<typename Buffer>
file::complete_type file::read(offset_type offset, Buffer& buf) {
  return read(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)));
}

template<typename Buffer>
file::complete_type file::write(offset_type offset, const Buffer& buf) {
  return write(offset, std::data(buf), static_cast<unsigned long>(std::size(buf)));
}

}  // namespace base