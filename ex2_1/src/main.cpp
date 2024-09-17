// #include <print>
#include <iostream>
#include <numbers>

#include <functional>
#include <chrono>
auto timer_ms_with_return(auto&& func, auto&&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::invoke(func, args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::make_pair(result, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

auto timer_ms(auto&& func, auto&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    func(args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

constexpr unsigned N = 1000000000;
constexpr double h = std::numbers::pi / 2.0 / N;
constexpr double par1 = 1.0 / 6;
constexpr double par2 = 1.0 / 20;
constexpr double par3 = 1.0 / 42;

inline constexpr double ex2_1_fx(double x)
{
    // $$f(x)=x(1-\frac{x^2}{3!}(1-\frac{x^2}{5!}(1-\frac{x^2}{7!})))$$
    double x2 = x*x;
    return x * (1 - x2 * par1 * (1 - x2 * par2 * (1 - x2 * par3)));
}

#define FOR_ONLY
#define EXPAND
#define T_INLINE
#define VECTORIZE

#ifdef FOR_ONLY
double s1 = 0;
void ex2_1_s_for_only()
{
    for (unsigned k1 = 0; k1 < N; ++k1)
    {
        s1 += ex2_1_fx(k1 * h) * h;
    }
}
#endif

// double s2 = 0;
// unsigned k2 = 0;
// void recursive_help()
// {
//     s2 += ex2_1_fx(k2 * h) * h;
//     ++k2;
// }
// double ex2_1_s_expand(double x)
// {
//     // $$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
//     // 其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$
//     repeat<N>(recursive_help);
//     return s2;
// }

#ifdef EXPAND
double s3 = 0;
void ex2_1_s_half_expand()
{
    for (unsigned k3 = 0; k3 < N; k3 += 4)
    {
        s3 += ex2_1_fx(k3 * h) * h;
        s3 += ex2_1_fx((k3+1) * h) * h;
        s3 += ex2_1_fx((k3+2) * h) * h;
        s3 += ex2_1_fx((k3+3) * h) * h;
    }
}
#endif

#ifdef VECTORIZE
#include <immintrin.h> // For AVX intrinsics

double vector_result {0.0};

void ex2_1_s_vector() {
    // Assumes N is a multiple of 4 for simplicity
    size_t i;
    // double vector_result = 0.0;

    // Initialize an AVX2 register to accumulate results
    __m256d acc = _mm256_setzero_pd();

    // Process 4 elements per iteration using AVX2
    for (i = 0; i < N; i += 4) {
        // Load 4 values of (k1 * h) into an AVX2 register
        __m256d k1_h = _mm256_set_pd(i* h, (i+1)* h, (i+2)* h, (i+3)* h);

        // Compute function values (ex2_1_fx) for 4 different k1 * h
        __m256d fx_vals = _mm256_set_pd(ex2_1_fx((i+3) * h), ex2_1_fx((i+2) * h), ex2_1_fx((i+1) * h), ex2_1_fx(i * h));

        // Multiply function values by h
        fx_vals = _mm256_mul_pd(fx_vals, _mm256_set1_pd(h));

        // Accumulate the results
        acc = _mm256_add_pd(acc, fx_vals);
    }

    // Horizontal sum of the accumulated results
    double temp[4];
    _mm256_storeu_pd(temp, acc);
    for (int j = 0; j < 4; ++j) {
        vector_result += temp[j];
    }

    // Handle remaining elements if N is not a multiple of 4
    for (; i < N; ++i) {
        vector_result += ex2_1_fx(i * h) * h;
    }
}
#endif

#ifdef T_INLINE
double ex2_1_s_inline()
{
    double s1 = 0;
    for (unsigned k1 = 0; k1 < N; ++k1)
    {
        double pa1 = k1 * h;
        double pa12 = pa1 * pa1;
        s1 += pa1 * (1 - pa12 * par1 * (1 - pa12 * par2 * (1 - pa12 * par3))) * h;
    }
    return s1;
}
#endif

int main(int argc, char** argv) {
    #ifdef FOR_ONLY
    auto time = timer_ms(ex2_1_s_for_only);
    // std::print("ex2_1_s_for_only(1) = {}, time = {}ms\n", result, time);
    std::cout << "ex2_1_s_for_only(1) = " << s1 << ", time = " << time << "ms" << std::endl;
    #endif

    // auto [result2, time2] = timer_ms(ex2_1_s_expand, 1);
    // std::print("ex2_1_s_expand(1) = {}, time = {}ms\n", result2, time2);

    #ifdef EXPAND
    auto time3 = timer_ms(ex2_1_s_half_expand);
    // std::print("ex2_1_s_half_expand(1) = {}, time = {}ms\n", result3, time3);
    std::cout << "ex2_1_s_half_expand(1) = " << s3 << ", time = " << time3 << "ms" << std::endl;
    #endif

    #ifdef T_INLINE
    auto [result4, time4] = timer_ms_with_return(ex2_1_s_inline);
    // std::print("ex2_1_s_inline(1) = {}, time = {}ms\n", result4, time4);
    std::cout << "ex2_1_s_inline(1) = " << result4 << ", time = " << time4 << "ms" << std::endl;
    #endif

    #ifdef VECTORIZE
    auto time5 = timer_ms(ex2_1_s_vector);
    // std::print("ex2_1_s_vector(1) = {}, time = {}ms\n", result5, time5);
    std::cout << "ex2_1_s_vector(1) = " << vector_result << ", time = " << time5 << "ms" << std::endl;
    #endif

    // auto start = std::chrono::high_resolution_clock::now();
    // auto result5 = ex2_1_s_inline(1);
    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "ex2_1_s_inline(1) = " << result5 << ", time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}
