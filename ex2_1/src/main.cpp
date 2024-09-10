#include <cmath>
#include <functional>
#include <iostream>
#include <print>
// #include <concepts>

// 辅助类，用于递归展开函数调用
template <unsigned N>
struct RepeatHelper {
    template <std::invocable Callable>
    static void call(Callable&& func) {
        if constexpr (N > 0) {
            func();
            RepeatHelper<N / 2>::call(func);
            RepeatHelper<N - N / 2 - 1>::call(func);
        }
    }
};

// 基本情况：当 N 为 0 时，不再调用
template <>
struct RepeatHelper<0> {
    template <std::invocable Callable>
    static void call(Callable&&) {
        // 不做任何操作
    }
};

// 模板函数，接受一个可调用对象和展开次数
template <unsigned N, std::invocable Callable>
constexpr void repeat(Callable&& func) {
    RepeatHelper<N>::call(std::forward<Callable>(func));
}

constexpr unsigned N = 1000000000;

constexpr double ex2_1_fx(double x)
{
    // $$f(x)=x(1-\frac{x^2}{3!}(1-\frac{x^2}{5!}(1-\frac{x^2}{7!})))$$
    double x2 = std::pow(x, 2);
    return x * (1 - x2 / 6 * (1 - x2 / 20 * (1 - x2 / 48)));
}

constexpr double ex2_1_s_for_only(double x)
{
    // $$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
    // 其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$
    double h = M_PI / 2 / N;
    double s = 0;
    for (unsigned k = 0; k < N; ++k)
    {
        s += ex2_1_fx(k * h) * h;
    }
    return s;
}

constexpr double ex2_1_s_expand(double x)
{
    // $$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
    // 其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$
    double h = M_PI / 2 / N;
    double s = 0;
    unsigned k = 0;
    auto func = [&]() {
        s += ex2_1_fx(k * h) * h;
        ++k;
    };
    repeat<N>(func);
    return s;
}

// time tester
#include <chrono>
auto timer_ms(auto&& func, auto&&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::invoke(func, args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::make_pair(result, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}

int main(int argc, char** argv) {
    auto [result, time] = timer_ms(ex2_1_s_for_only, 1);
    std::print("ex2_1_s_for_only(1) = {}, time = {}ms\n", result, time);

    auto [result2, time2] = timer_ms(ex2_1_s_expand, 1);
    std::print("ex2_1_s_expand(1) = {}, time = {}ms\n", result2, time2);
}
