#include "future_val.h"

namespace base {

bad_future_val::bad_future_val()
  : std::logic_error("Bad future value") {
}

}  // namespace base