#include "exceptions.h"

namespace base {

no_future_state::no_future_state()
  : future_error("No state") {
}

abandoned_promise::abandoned_promise()
  : future_error("Promise abandoned") {
}

future_already_retrieved::future_already_retrieved()
  : future_error("Future already retrieved") {
}

}  // namespace base