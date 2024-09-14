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

| CPU |Apple M1| 架构 | arm64 |
| --- | --- | --- | --- |
| 主频 | 3.2/2.6 GHz | 核心数 | 8-core |
| 操作系统 |macOS 15.0 Beta| 编译器 | Homebrew GCC 14.2.0 |
| 三级缓存 | 8 MB | 内存速度 | 4266 MT/s |
> 实验用软硬件平台参数

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
4. 通过研究产生的汇编，可以发现编译器在 O3 release 优化的情况下，将整个ex_1_2_for_only函数所用到的寄存器压缩到了10个，而手动优化破坏了编译器的优化，反而导致了寄存器开销的增多与缓存的未命中，时间开销增加。
```asm
ex2_1_s_for_only(double):
        mov     eax, DWORD PTR k1[rip]
        movsd   xmm5, QWORD PTR s1[rip]
        cmp     eax, 999999999
        ja      .L1
        movsd   xmm6, QWORD PTR .LC0[rip]
        movsd   xmm9, QWORD PTR .LC1[rip]
        movsd   xmm4, QWORD PTR .LC2[rip]
        movsd   xmm7, QWORD PTR .LC4[rip]
        movsd   xmm8, QWORD PTR .LC3[rip]
.L3:
        pxor    xmm1, xmm1
        movapd  xmm3, xmm4
        cvtsi2sd        xmm1, eax
        add     eax, 1
        mulsd   xmm1, xmm6
        movapd  xmm0, xmm1
        mulsd   xmm0, xmm1
        movapd  xmm2, xmm0
        divsd   xmm2, xmm9
        subsd   xmm3, xmm2
        movapd  xmm2, xmm0
        divsd   xmm2, xmm8
        divsd   xmm0, xmm7
        mulsd   xmm3, xmm2
        movapd  xmm2, xmm4
        subsd   xmm2, xmm3
        mulsd   xmm2, xmm0
        movapd  xmm0, xmm4
        subsd   xmm0, xmm2
        mulsd   xmm0, xmm1
        mulsd   xmm0, xmm6
        addsd   xmm5, xmm0
        cmp     eax, 1000000000
        jne     .L3
        mov     DWORD PTR k1[rip], 1000000000
        movsd   QWORD PTR s1[rip], xmm5
.L1:
        movapd  xmm0, xmm5
        ret
```
