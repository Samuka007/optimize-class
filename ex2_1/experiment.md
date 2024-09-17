# 实验报告2.1
## 问题描述
对于函数 
$$f(x)=x-\frac{x^3}{3!}+\frac{x^5}{5!}-\frac{x^7}{7!}$$ 
使用双精度浮点计算 
$$s=\int_0^{π/2}f(x)dx \approx {\sum}_{k=0}^{N-1}f(kh)h$$
其中 $h$ 和 $N$ 满足 $h {\times} N=\frac{\pi}{2}$ 。
设定 $N = 10^9$ ，请完成两个版本的程序，并对比性能。

1. 使用Horner算法计算多项式 $f(x)$ 的值，并计算s。
2. 在上一方法的基础上，使用循环展开方法加速计算过程。

## 实验过程
**使用Horner算法简化 $f(x)$**

1. 将 $f(x)$ 展开得到
$$f(x)=x(1-\frac{x^2}{3!}(1-\frac{x^2}{4\times5}(1-\frac{x^2}{6\times7})))$$

2. 实现、展开与汇编可以参考：`https://godbolt.org/z/rev3Mvfas`

<!-- | CPU |Apple M1| 架构 | arm64 |
| --- | --- | --- | --- |
| 主频 | 3.2/2.6 GHz | 核心数 | 8-core |
| 操作系统 |macOS 15.0 Beta| 编译器 | Homebrew GCC 14.2.0 |
| 三级缓存 | 8 MB | 内存速度 | 4266 MT/s |
> 实验用软硬件平台参数 -->

## 实验结果

在参数 -O0, debug 模式下

| 方法 | 时间(ms) |
| --- | --- |
pure loop | 6092
unroll all | 22786
half unroll | 5766

在参数 -O1, debug -funroll-loops 下
| 方法 | 时间(ms) |
| --- | --- |
pure loop | 5828
unroll all | 5703
half unroll | 2623

在参数 -O3, release 模式下
| 方法 | 时间(ms) |
| --- | --- |
pure loop | 1864
unroll all | 5734
half unroll | 2371


## 实验分析
1. 在任何情况下，在遇到了 $N = 10^9$ 的量级的循环时，完全循环展开的方法并没有带来性能提升，反而时间开销与空间开销剧增。说明，完全展开在遇到大量循环时，并不是一个好的优化方法。猜测是因为完全展开后，代码量过大，导致了缓存未命中，反而增加了时间开销。
2. 在 O0 与 O1 -funroll-loops 优化的情况下，半展开的方法都有着5%～54%的性能提升，说明在无优化与轻度优化的情况下，半展开能起到一定的优化效果。
3. 在 O3 release 优化的情况下，两种手动优化的方法都成了小丑，时间开销都大于未优化的方法，编译器对于finite loop的优化效果是非常好的，手动优化反而会带来性能下降。

```output
PS C:\Users\Administrator\workspace\optimize-class\ex2_1> xmake -rv
[ 50%]: cache compiling.release src\main.cpp
gcc -c -m64 -fvisibility=hidden -fvisibility-inlines-hidden -O3 -std=c++23 -march=native -fopt-info-vec-missed -fopt-info-vec -DNDEBUG -o build\.objs\ex2_1\mingw\x86_64\release\src\main.cpp.obj src\main.cpp
src\main.cpp:44:30: missed: couldn't vectorize loop
src\main.cpp:42:6: missed: not vectorized: unsupported data-type double
src\main.cpp:70:30: missed: couldn't vectorize loop
src\main.cpp:68:6: missed: not vectorized: unsupported data-type double
src\main.cpp:111:23: optimized: loop vectorized using 32 byte vectors
src\main.cpp:94:19: missed: couldn't vectorize loop
src\main.cpp:85:6: missed: not vectorized: unsupported data-type size_t
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/avxintrin.h:1260:54: optimized: basic block part vectorized using 32 byte vectors
src\main.cpp:126:30: missed: couldn't vectorize loop
src\main.cpp:124:6: missed: not vectorized: unsupported data-type double
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/c++/bits/locale_facets.h:884:21: missed: statement clobbers memory: std::ctype<char>::_M_widen_init (_14);
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/c++/bits/locale_facets.h:885:23: missed: statement clobbers memory: _27 = OBJ_TYPE_REF(_25;_14->6B) (_14, 10);
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/c++/ostream:742:28: missed: statement clobbers memory: _8 = std::basic_ostream<char>::put (__os_1(D), _7);
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/c++/ostream:764:24: missed: statement clobbers memory: std::basic_ostream<char>::flush (_8);
C:/w64devkit/lib/gcc/x86_64-w64-mingw32/14.1.0/include/c++/bits/basic_ios.h:50:18: missed: statement clobbers memory: std::__throw_bad_cast ();
src\main.cpp:16:57: missed: statement clobbers memory: start = std::chrono::_V2::system_clock::now ();
[ 75%]: linking.release ex2_1.exe
g++ -o build\mingw\x86_64\release\ex2_1.exe build\.objs\ex2_1\mingw\x86_64\release\src\main.cpp.obj -m64 -s
[100%]: build ok, spent 2.734s
PS C:\Users\Administrator\workspace\optimize-class\ex2_1> xmake run
ex2_1_s_for_only(1) = 0.999975, time = 1138ms
ex2_1_s_half_expand(1) = 0.999975, time = 1085ms
ex2_1_s_inline(1) = 0.999975, time = 1085ms
ex2_1_s_vector(1) = 0.999975, time = 472ms
```
