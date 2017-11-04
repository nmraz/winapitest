#pragma once

#include <stdexcept>

namespace base {

class future_error : public std::logic_error {
public:
  using std::logic_error::logic_error;
};


// common

class no_future_state : public future_error {
public:
  no_future_state();
};


// future-related

class continuation_already_set : public future_error {
public:
  continuation_already_set();
};


// promise-related

class abandoned_promise : public future_error {
public:
  abandoned_promise();
};

class promise_already_fulfilled : public future_error {
public:
  promise_already_fulfilled();
};

class future_already_retrieved : public future_error {
public:
  future_already_retrieved();
};

}  // namespace base