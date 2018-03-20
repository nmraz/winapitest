#include "expected.h"

namespace base {

bad_expected_access::bad_expected_access()
  : std::logic_error("Bad future value") {
}

}  // namespace base