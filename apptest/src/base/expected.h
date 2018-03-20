#pragma once

#include <exception>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace base {

class bad_expected_access : public std::logic_error {
public:
  bad_expected_access();
};


namespace impl {

template<typename T>
class expected_base {
public:
  void set_exception(std::exception_ptr exc);
  template<typename Exc>
  void set_exception(Exc&& exc);

  void reset();

  bool empty() const { return val_.index() == 0; }
  bool has_value() const { return val_.index() == 1; }
  bool has_exception() const { return val_.index() == 2; }

protected:
  std::variant<std::monostate, T, std::exception_ptr> val_;
};


template<typename T>
void expected_base<T>::set_exception(std::exception_ptr exc) {
  val_.template emplace<2>(std::move(exc));
}

template<typename T>
template<typename Exc>
void expected_base<T>::set_exception(Exc&& exc) {
  set_exception(std::make_exception_ptr(std::forward<Exc>(exc)));
}

template<typename T>
void expected_base<T>::reset() {
  val_.template emplace<0>();
}

}  // namespace impl


template<typename T>
class expected : public impl::expected_base<T> {
public:
  template<typename U>
  void set_value(U&& val);

  T& get() &;
  const T& get() const &;
  T&& get() && ;
  const T&& get() const &&;
};


template<typename T>
template<typename U>
void expected<T>::set_value(U&& val) {
  this->val_.template emplace<1>(std::forward<U>(val));
}

template<typename T>
T& expected<T>::get() & {

  // note: we can't use std::visit since T may be std::monostate or std::exception_ptr
  if (this->has_value()) {
    return std::get<1>(this->val_);
  } else if (this->has_exception()) {
    std::rethrow_exception(std::get<2>(this->val_));
  } else {
    throw bad_expected_access();
  }
}

template<typename T>
const T& expected<T>::get() const & {
  if (this->has_value()) {
    return std::get<1>(this->val_);
  } else if (this->has_exception()) {
    std::rethrow_exception(std::get<2>(this->val_));
  } else {
    throw bad_expected_access();
  }
}

template<typename T>
T&& expected<T>::get() && {
  if (this->has_value()) {
    return std::get<1>(std::move(this->val_));
  } else if (this->has_exception()) {
    std::rethrow_exception(std::get<2>(std::move(this->val_)));
  } else {
    throw bad_expected_access();
  }
}

template<typename T>
const T&& expected<T>::get() const && {
  if (this->has_value()) {
    return std::get<1>(std::move(this->val_));
  } else if (this->has_exception()) {
    std::rethrow_exception(std::get<2>(std::move(this->val_)));
  } else {
    throw bad_expected_access();
  }
}


template<>
class expected<void> : public impl::expected_base<char> {
public:
  void set_value() {
    val_.emplace<1>();
  }

  void get() const {
    if (has_exception()) {
      std::rethrow_exception(std::get<2>(val_));
    } else if (!has_value()) {
      throw bad_expected_access();
    }
  }
};

}  // namespace base