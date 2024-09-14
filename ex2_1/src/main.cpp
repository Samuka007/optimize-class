#include <functional>
#include <print>
#include <numbers>

// 辅助类，用于递归展开函数调用
template <unsigned N>
struct RepeatHelper {
    template <typename Callable, typename...Args>
    requires std::invocable<Callable, Args...>
    static constexpr void call(Callable&& func, Args&&...args) {
        if constexpr (N > 0) {
            std::invoke(std::forward<Callable>(func), std::forward<Args>(args)...);
            RepeatHelper<N / 2>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
            RepeatHelper<N - N / 2 - 1>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
        } else if constexpr (N == 0) {
            // 不做任何操作
        }
    }
};

// 模板函数，接受一个可调用对象和展开次数
template <unsigned N, typename Callable, typename...Args>
constexpr void repeat(Callable&& func, Args&&...args) {
    RepeatHelper<N>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
}

constexpr unsigned N = 1000000000;
constexpr double h = std::numbers::pi / 2.0 / N;

inline constexpr double ex2_1_fx(double x)
{
    // $$f(x)=x(1-\frac{x^2}{3!}(1-\frac{x^2}{5!}(1-\frac{x^2}{7!})))$$
    double x2 = x*x;
    return x * (1 - x2 / 6 * (1 - x2 / 20 * (1 - x2 / 42)));
}

double s1 = 0;
unsigned k1 = 0;

double ex2_1_s_for_only(double x)
{
    // $$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
    // 其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$
    for (; k1 < N; ++k1)
    {
        s1 += ex2_1_fx(k1 * h) * h;
    }
    return s1;
}

double s2 = 0;
unsigned k2 = 0;

void recursive_help()
{
    s2 += ex2_1_fx(k2 * h) * h;
    ++k2;
}

double ex2_1_s_expand(double x)
{
    // $$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
    // 其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$
    repeat<N>(recursive_help);
    return s2;
}

double s3 = 0;
unsigned k3 = 0;

double ex2_1_s_half_expand(double x)
{
    while (k3 < N)
    {
        s3 += ex2_1_fx(k3++ * h) * h;
        s3 += ex2_1_fx(k3++ * h) * h;
        s3 += ex2_1_fx(k3++ * h) * h;
        s3 += ex2_1_fx(k3++ * h) * h;
    }
    return s3;
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

    // auto [result2, time2] = timer_ms(ex2_1_s_expand, 1);
    // std::print("ex2_1_s_expand(1) = {}, time = {}ms\n", result2, time2);

    auto [result3, time3] = timer_ms(ex2_1_s_half_expand, 1);
    std::print("ex2_1_s_half_expand(1) = {}, time = {}ms\n", result3, time3);
}
