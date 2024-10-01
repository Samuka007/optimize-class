#ifndef MATRIX2D_HPP
#define MATRIX2D_HPP

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <utility>
#include "baseline.hpp"

template <typename T, std::size_t X, std::size_t Y>
requires std::is_arithmetic_v<T>
class Matrix2D {
    private:
    T m_data[X][Y];

    public:
    Matrix2D() : m_data {} {}

    // Matrix2D(const T* arr) {
    //     for (std::size_t i = 0; i < X; ++i) {
    //         for (std::size_t j = 0; j < Y; ++j) {
    //             data[i][j] = arr[i * Y + j];
    //         }
    //     }
    // }

    Matrix2D(T&& arr) : m_data(std::move(arr)) {}

    T& operator()(std::size_t i, std::size_t j) {
        return m_data[i][j];
    }

    const T& operator()(std::size_t i, std::size_t j) const {
        return m_data[i][j];
    }

    template <std::size_t Z>
    Matrix2D<T, X, Y> operator*(const Matrix2D<T, Y, Z>& other) const {
        Matrix2D<T, X, Z> result;
        if constexpr (X == Y && Y == Z) {
            matrix_multiply(reinterpret_cast<const float*>(this->m_data), 
                            reinterpret_cast<const float*>(other.m_data), 
                            reinterpret_cast<float*>(result.m_data), X);
        } else {
            for (std::size_t i = 0; i < X; ++i) {
                for (std::size_t j = 0; j < X; ++j) {
                    result(i, j) = 0.0;
                    for (std::size_t k = 0; k < Y; ++k) {
                        result(i, j) += m_data[i][k] * other(k, j);
                    }
                }
            }
        }
        return result;
    }

    T& data() {
        return m_data[0][0];
    }

    const T& data() const {
        return m_data[0][0];
    }

    T trace() const {
        T sum = 0.0f;
        for (std::size_t i = 0; i < std::min(X, Y); ++i) {
            sum += m_data[i][i];
        }
        return sum;
    }
};


template <std::size_t N>
auto generate_matrix_pair(float seed) {
    Matrix2D<float, N, N> a_matrix, b_matrix;
    float* a = &a_matrix.data();
    float* b = &b_matrix.data();
    matrix_gen(a, b, N, seed);
    return std::make_pair(a_matrix, b_matrix);
}

// #define MATRIX_BRICK
#ifdef MATRIX_BRICK

/**
    cache line (l1 cache) and l2 cache is per core
 */

#include <new>
constexpr std::size_t cache_line_size = 
#ifdef __cpp_lib_hardware_interference_size
std::max(
    std::hardware_constructive_interference_size,
    std::hardware_destructive_interference_size
);
#else
    #ifdef L1_CACHE
    L1_CACHE;
    #else
    64 /* KB */ << 10;
    #endif
#endif

constexpr std::size_t l2_cache_size =
#ifdef L2_CACHE
L2_CACHE;
#else
256 /* KB */ << 10;
#endif

template <std::size_t X>
class Matrix2D<float, X, X> {
    private:
    float m_data[X][X];

    public:
    Matrix2D<float, X, X> operator*(const Matrix2D<float, X, X>& other) const;

    float& data() {
        return m_data[0][0];
    }

    const float& data() const {
        return m_data[0][0];
    }

    // 获得矩阵的迹
    float trace() const {
        float sum = 0.0f;
        for (std::size_t i = 0; i < X; ++i) {
            sum += m_data[i][i];
        }
        return sum;
    }
};

#ifndef MATRIX_CONCURRENCY // 仅分块计算

template <std::size_t X>
Matrix2D<float, X, X> 
Matrix2D<float, X, X>::operator*(const Matrix2D<float, X, X>& other) const {
    Matrix2D<float, X, X> result;
    constexpr std::size_t block_size = cache_line_size / sizeof(float);

    for (std::size_t i = 0; i < X; i += block_size) {
        for (std::size_t j = 0; j < X; j += block_size) {
            for (std::size_t k = 0; k < X; k += block_size) {
                for (std::size_t ii = i; ii < std::min(i + block_size, X); ++ii) {
                    for (std::size_t jj = j; jj < std::min(j + block_size, X); ++jj) {
                        float sum = 0.0f;
                        for (std::size_t kk = k; kk < std::min(k + block_size, X); ++kk) {
                            sum += m_data[ii][kk] * other(kk, jj);
                        }
                        result(ii, jj) += sum;
                    }
                }
            }
        }
    }
    return result;
}

#else

#include <thread>
const std::size_t hardware_concurrency = std::thread::hardware_concurrency();

#endif

#endif

#endif