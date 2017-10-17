#include "exceptions.h"

namespace base {

no_state::no_state()
  : future_error("No state") {
}

continuation_already_set::continuation_already_set()
  : future_error("Future continuation already set") {
}

abandoned_promise::abandoned_promise()
  : future_error("Promise abandoned") {
}

promise_already_fulfilled::promise_already_fulfilled()
  : future_error("Promise already fulfilled") {
}

future_already_retrieved::future_already_retrieved()
  : future_error("Future already retrieved") {
}

}  // namespace base