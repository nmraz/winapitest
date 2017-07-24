#pragma once

namespace gfx {

template<typename Rep, int M, int N>
struct matrix {
	Rep data[M][N];

	constexpr Rep& get(int row, int col);
	constexpr const Rep& get(int row, int col) const;
};


template<typename Rep, int M, int N>
constexpr Rep& matrix<Rep, M, N>::get(int row, int col) {
	return data[row][col];
}

template<typename Rep, int M, int N>
constexpr const Rep& matrix<Rep, M, N>::get(int row, int col) const {
	return data[row][col];
}


template<typename Rep, int M, int N, int P>
constexpr matrix<Rep, M, P> operator*(const matrix<Rep, M, N>& lhs, const matrix<Rep, N, P>& rhs) {
	matrix<Rep, M, P> ret{};

	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < P; ++j) {
			Rep total = 0;
			for (int k = 0; k < N; ++k) {
				total += lhs.get(i, k) * rhs.get(k, j);
			}
			ret.get(i, j) = total;
		}
	}

	return ret;
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