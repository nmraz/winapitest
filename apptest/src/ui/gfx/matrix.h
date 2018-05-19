#pragma once

namespace gfx {

template<typename T, int M, int N>
class matrix {
public:
  template<typename... Args, typename = std::enable_if_t<std::conjunction_v<std::is_convertible<Args, T>...>>>
  constexpr matrix(Args... args) : data_{ static_cast<T>(args)... } {}

  constexpr T& operator()(int row, int col);
  constexpr const T& operator()(int row, int col) const;

private:
  T data_[M][N];
};


template<typename T, int M, int N>
constexpr T& matrix<T, M, N>::operator()(int row, int col) {
  return data_[row][col];
}

template<typename T, int M, int N>
constexpr const T& matrix<T, M, N>::operator()(int row, int col) const {
  return data_[row][col];
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


template<int M, int N>
using matf = matrix<float, M, N>;

template<int M, int N>
using mati = matrix<int, M, N>;

using mat32f = matf<3, 2>;
using mat33f = matf<3, 3>;

using mat32i = mati<3, 2>;
using mat33i = mati<3, 3>;

}  // namespace gfx