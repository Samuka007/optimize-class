#include <iostream>
#include <chrono>
#include <numbers>

constexpr unsigned N = 1000000000;
constexpr double h = std::numbers::pi / 2.0 / N;
constexpr double par1 = 1.0 / 6;
constexpr double par2 = 1.0 / 20;
constexpr double par3 = 1.0 / 42;

inline constexpr double ex2_1_fx(double x)
{
    double x2 = x*x;
    return x * (1 - x2 * par1 * (1 - x2 * par2 * (1 - x2 * par3)));
}

#include <immintrin.h> // For AVX intrinsics

double vector_result {0.0};
const __m256d one = _mm256_set1_pd(1.0);
const __m256d param1 = _mm256_set1_pd(par1);
const __m256d param2 = _mm256_set1_pd(par2);
const __m256d param3 = _mm256_set1_pd(par3);
const __m256d h_vec = _mm256_set1_pd(h);

inline __m256d fx_inline(size_t i)
{
    __m256d x_vec = _mm256_set_pd(
        (i+3), 
        (i+2), 
        (i+1), 
        i
    );

    x_vec = _mm256_mul_pd(x_vec, h_vec);

    __m256d x_vec_sqr = _mm256_mul_pd(x_vec, x_vec);

    __m256d x_sqr_times_param3 = _mm256_mul_pd(
        x_vec_sqr, 
        param3
    );

    __m256d x_sqr_times_param2 = _mm256_mul_pd(
        x_vec_sqr, 
        param2
    );

    __m256d x_sqr_times_param1 = _mm256_mul_pd(
        x_vec_sqr, 
        param1
    );

    __m256d param2_param3 = _mm256_mul_pd(
        x_sqr_times_param2,
        _mm256_sub_pd(one, x_sqr_times_param3)
    );

    __m256d param1_param2_param3 = _mm256_mul_pd(
        x_sqr_times_param1,
        _mm256_sub_pd(one, param2_param3)
    );

    __m256d fx_vals = _mm256_mul_pd(
        x_vec,
        _mm256_sub_pd(one, param1_param2_param3)
    );

    return _mm256_mul_pd(fx_vals, h_vec);
}

inline void ex2_1_s_vector() 
{
    __m256d acc = _mm256_setzero_pd();
    size_t i {0};

    for (; i < N; i += 16) {
        acc = _mm256_add_pd(acc, fx_inline(i));
        acc = _mm256_add_pd(acc, fx_inline(i+4));
        acc = _mm256_add_pd(acc, fx_inline(i+8));
        acc = _mm256_add_pd(acc, fx_inline(i+12));
    }

    // Horizontal sum of the accumulated results
    double temp[4];
    _mm256_storeu_pd(temp, acc);
    for (int j = 0; j < 4; ++j) {
        vector_result += temp[j];
    }

    for (; i < N; ++i) {
        vector_result += ex2_1_fx(i * h) * h;
    }
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    ex2_1_s_vector();
    auto end = std::chrono::high_resolution_clock::now();
    auto time5 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "ex2_1_s_vector(1) = " << vector_result << ", time = " << time5 << "ms" << std::endl;
}