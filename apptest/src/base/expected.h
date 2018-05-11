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
  static_assert(!std::is_same_v<T, std::exception_ptr>, "expected<std::exception_ptr> is not supported. Use expected<void> insted.");
  static_assert(!std::is_same_v<T, std::monostate>, "expected<std::monostate> is not supported. Use expected<void> insted.");

  void set_exception(std::exception_ptr exc);
  template<typename Exc>
  void set_exception(Exc&& exc);

  void reset();

  bool empty() const { return std::holds_alternative<std::monostate>(val_); }
  bool has_value() const { return std::holds_alternative<T>(val_); }
  bool has_exception() const { return std::holds_alternative<std::exception_ptr>(val_); }

  std::exception_ptr get_exception() const {
    const std::exception_ptr* ptr = std::get_if<std::exception_ptr>(&val_);
    return ptr ? *ptr : nullptr;
  }

protected:
  [[noreturn]] void rethrow_exception() const {
    if (auto exc = get_exception()) {
      std::rethrow_exception(exc);
    }
    throw bad_expected_access();
  }

  std::variant<std::monostate, T, std::exception_ptr> val_;
};


template<typename T>
void expected_base<T>::set_exception(std::exception_ptr exc) {
  val_ = std::move(exc);
}

template<typename T>
template<typename Exc>
void expected_base<T>::set_exception(Exc&& exc) {
  set_exception(std::make_exception_ptr(std::forward<Exc>(exc)));
}

template<typename T>
void expected_base<T>::reset() {
  val_ = std::monostate{};
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
  this->val_ = static_cast<T>(std::forward<U>(val));
}

template<typename T>
T& expected<T>::get() & {
  if (this->has_value()) {
    return std::get<T>(this->val_);
  }
  this->rethrow_exception();
}

template<typename T>
const T& expected<T>::get() const & {
  if (this->has_value()) {
    return std::get<T>(this->val_);
  }
  this->rethrow_exception();
}

template<typename T>
T&& expected<T>::get() && {
  if (this->has_value()) {
    return std::get<T>(std::move(this->val_));
  }
  this->rethrow_exception();
}

template<typename T>
const T&& expected<T>::get() const && {
  if (this->has_value()) {
    return std::get<T>(std::move(this->val_));
  }
  this->rethrow_exception();
}


template<>
class expected<void> : public impl::expected_base<char> {
public:
  void set_value() {
    val_ = char{};
  }

  void get() const {
    if (!has_value()) {
      rethrow_exception();
    }
  }
};

}  // namespace base