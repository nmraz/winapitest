#pragma once

namespace gfx {

template<typename T, int M, int N>
struct matrix {
  T data[M][N];

  constexpr T& get(int row, int col);
  constexpr const T& get(int row, int col) const;
};


template<typename T, int M, int N>
constexpr T& matrix<T, M, N>::get(int row, int col) {
  return data[row][col];
}

template<typename T, int M, int N>
constexpr const T& matrix<T, M, N>::get(int row, int col) const {
  return data[row][col];
}


template<typename T, int M, int N, int P>
constexpr matrix<T, M, P> operator*(const matrix<T, M, N>& lhs, const matrix<T, N, P>& rhs) {
  matrix<T, M, P> ret{};

  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < P; ++j) {
      T total = 0;
      for (int k = 0; k < N; ++k) {
        total += lhs.get(i, k) * rhs.get(k, j);
      }
      ret.get(i, j) = total;
    }
  }

  return ret;
}

template<typename T, int M>
constexpr matrix<T, M, M>& operator*=(matrix<T, M, M>& lhs, const matrix<T, M, M>& rhs) {
  return lhs = lhs * rhs;
}


template<typename T, int M, int N>
constexpr bool operator==(const matrix<T, M, N>& lhs, const matrix<T, M, N>& rhs) {
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      if (lhs.get(i, j) != rhs.get(i, j)) {
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