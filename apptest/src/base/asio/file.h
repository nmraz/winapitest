#pragma once

#include "base/promise.h"
#include "base/win/scoped_handle.h"
#include <cstdint>
#include <functional>
#include <iterator>
#include <string_view>
#include <system_error>

namespace base {

class file {
public:
  enum flags {
    open_only = 1 << 0,  // open only if exists
    create_only = 1 << 1,  // create only if doesn't exist

    open_always = 1 << 2,  // open or create
    create_always = 1 << 3,  // open and trucate or create
    
    in = 1 << 4,  // read
    out = 1 << 5  // write
  };

  using offset_type = std::int64_t;
  using complete_type = promise<unsigned long>;

  file() = default;
  file(std::string_view name, int open_flags);

  void open(std::string_view name, int open_flags);
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