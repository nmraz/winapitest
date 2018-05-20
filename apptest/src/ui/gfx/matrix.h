#pragma once

namespace gfx {
namespace impl {

template<typename T>
constexpr bool is_matrix_expr = false;


template<typename E, typename S>
struct matrix_scalar_mul_expr {
  using element_type = typename E::element_type;

  static constexpr int rows = E::rows;
  static constexpr int cols = E::cols;

  constexpr element_type operator()(int i, int j) const {
    return static_cast<element_type>(expr(i, j) * scalar);
  }

  const E& expr;
  S scalar;
};

template<typename E, typename S>
constexpr bool is_matrix_expr<matrix_scalar_mul_expr<E, S>> = true;


template<typename E, typename S>
struct matrix_scalar_div_expr {
  using element_type = typename E::element_type;

  static constexpr int rows = E::rows;
  static constexpr int cols = E::cols;

  constexpr element_type operator()(int i, int j) const {
    return static_cast<element_type>(expr(i, j) / scalar);
  }

  const E& expr;
  S scalar;
};

template<typename E, typename S>
constexpr bool is_matrix_expr<matrix_scalar_div_expr<E, S>> = true;


template<typename L, typename R>
struct matrix_add_expr {
  using element_type = typename L::element_type;

  static constexpr int rows = L::rows;
  static constexpr int cols = L::cols;

  constexpr element_type operator()(int i, int j) const {
    return lhs(i, j) + rhs(i, j);
  }

  const L& lhs;
  const R& rhs;
};

template<typename L, typename R>
constexpr bool is_matrix_expr<matrix_add_expr<L, R>> = true;


template<typename E1, typename E2, typename = void>
struct are_compatible_matrix_exprs_impl : std::false_type {};

template<typename E1, typename E2>
struct are_compatible_matrix_exprs_impl<E1, E2, std::enable_if_t<is_matrix_expr<E1> && is_matrix_expr<E2>>>
  : std::bool_constant<
      std::is_same_v<typename E1::element_type, typename E2::element_type>
      && E1::rows == E2::rows
      && E1::cols == E2::cols
  > {};

template<typename E1, typename E2>
constexpr bool are_compatible_matrix_exprs = are_compatible_matrix_exprs_impl<E1, E2>::value;

}  // namespace impl

template<typename T, int M, int N>
class matrix {
public:
  using element_type = T;

  static constexpr int rows = M;
  static constexpr int cols = N;

  template<typename... Args, typename = std::enable_if_t<std::conjunction_v<std::is_convertible<Args, T>...>>>
  constexpr matrix(Args... args) : data_{ static_cast<T>(args)... } {}

  template<typename E, typename = std::enable_if_t<impl::are_compatible_matrix_exprs<matrix, E>>>
  constexpr matrix(const E& expr);

  constexpr T& operator()(int row, int col);
  constexpr const T& operator()(int row, int col) const;

private:
  T data_[M][N];
};

namespace impl {

// work around MSVC bug when specializing namespaced variable templates
template<typename T, int M, int N>
constexpr bool is_matrix_expr<matrix<T, M, N>> = true;

}  // namespace impl


template<typename T, int M, int N>
template<typename E, typename>
constexpr matrix<T, M, N>::matrix(const E& expr)
  : data_{}  {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      (*this)(i, j) = expr(i, j);
    }
  }
}


template<typename T, int M, int N>
constexpr T& matrix<T, M, N>::operator()(int row, int col) {
  return data_[row][col];
}

template<typename T, int M, int N>
constexpr const T& matrix<T, M, N>::operator()(int row, int col) const {
  return data_[row][col];
}


template<typename E, typename S, typename = std::enable_if_t<impl::is_matrix_expr<E>>>
constexpr impl::matrix_scalar_mul_expr<E, S> operator*(const E& expr, S scalar) {
  return { expr, scalar };
}

template<typename E, typename S, typename = std::enable_if_t<impl::is_matrix_expr<E>>>
constexpr auto operator*(S scalar, const E& expr) {
  return expr * scalar;
}

template<typename E, typename S, typename = std::enable_if_t<impl::is_matrix_expr<E>>>
constexpr impl::matrix_scalar_div_expr<E, S> operator/(const E& expr, S scalar) {
  return { expr, scalar };
}


template<typename L, typename R, typename = std::enable_if_t<impl::are_compatible_matrix_exprs<L, R>>>
constexpr impl::matrix_add_expr<L, R> operator+(const L& lhs, const R& rhs) {
  return { lhs, rhs };
}

template<typename E, typename = std::enable_if_t<impl::is_matrix_expr<E>>>
constexpr auto operator-(const E& expr) {
  return -1 * expr;
}

template<typename L, typename R, typename = std::enable_if_t<impl::are_compatible_matrix_exprs<L, R>>>
constexpr auto operator-(const L& lhs, const R& rhs) {
  return lhs + -rhs;
}


template<typename T, int M, int N, int P>
constexpr matrix<T, M, P> operator*(const matrix<T, M, N>& lhs, const matrix<T, N, P>& rhs) {
  matrix<T, M, P> ret{};

  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < P; ++j) {
      T total = 0;
      for (int k = 0; k < N; ++k) {
        total += lhs(i, k) * rhs(k, j);
      }
      ret(i, j) = total;
    }
  }

  return ret;
}

template<typename T, int M, int N>
constexpr matrix<T, M, N>& operator*=(matrix<T, M, N>& lhs, const matrix<T, N, N>& rhs) {
  return lhs = lhs * rhs;
}


template<typename T, int M, int N>
constexpr bool operator==(const matrix<T, M, N>& lhs, const matrix<T, M, N>& rhs) {
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      if (lhs(i, j) != rhs(i, j)) {
        return false;
      }
    }
  }
  return true;
}

template<typename T, int M, int N>
constexpr bool operator!=(const matrix<T, M, N>& lhs, const matrix<T, M, N>& rhs) {
  return !(lhs == rhs);
}


template<typename E, typename = std::enable_if_t<impl::is_matrix_expr<E>>>
matrix(const E&) -> matrix<typename E::element_type, E::rows, E::cols>;


template<int M, int N>
using matf = matrix<float, M, N>;

template<int M, int N>
using mati = matrix<int, M, N>;

using mat32f = matf<3, 2>;
using mat33f = matf<3, 3>;

using mat32i = mati<3, 2>;
using mat33i = mati<3, 3>;

}  // namespace gfx