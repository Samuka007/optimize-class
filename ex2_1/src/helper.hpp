#pragma once

#include <functional>

// // 辅助类，用于递归展开函数调用
// template <unsigned N>
// struct RepeatHelper {
//     template <typename Callable, typename...Args>
//     requires std::invocable<Callable, Args...>
//     static constexpr void call(Callable&& func, Args&&...args) {
//         if constexpr (N > 0) {
//             std::invoke(std::forward<Callable>(func), std::forward<Args>(args)...);
//             RepeatHelper<N / 2>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
//             RepeatHelper<N - N / 2 - 1>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
//         } else if constexpr (N == 0) {
//             // 不做任何操作
//         }
//     }
// };

// // 模板函数，接受一个可调用对象和展开次数
// template <unsigned N, typename Callable, typename...Args>
// constexpr void repeat(Callable&& func, Args&&...args) {
//     RepeatHelper<N>::call(std::forward<Callable>(func), std::forward<Args>(args)...);
// }

// time tester
#include <chrono>
auto timer_ms_with_return(auto&& func, auto&&... args)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::invoke(func, args...);
    auto end = std::chrono::high_resolution_clock::now();
    return std::make_pair(result, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
}
