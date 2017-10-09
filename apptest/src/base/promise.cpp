#include "promise.h"

namespace base {

bad_promise_val::bad_promise_val()
  : std::logic_error("Bad promise value") {
}

abandoned_promise::abandoned_promise()
  : std::logic_error("Abandoned promise") {
}

}  // namespace base