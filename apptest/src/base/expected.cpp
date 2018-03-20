#include "expected.h"

namespace base {

bad_expected_access::bad_expected_access()
  : std::logic_error("Bad expected value") {
}

}  // namespace base