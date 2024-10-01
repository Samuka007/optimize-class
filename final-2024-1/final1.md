## 矩阵乘法
实现单精度浮点的矩阵乘法：$ C_{i,j} = \Sigma^{N}_{k=1} A_{i,k} \times B_{k,j} , (1 \le i,j \le N) $ ，并求矩阵 $ C $ 的迹。

### 程序输入输出要求
输入格式
```shell
Matrix_mul N seed

# 参数
# N: 矩阵大小
# seed: 输入的种子浮点数
```

输出格式
```shell
Trace

# Trace: 结果矩阵C的迹
```

### 矩阵产生函数
```c
/*
Input: a, b are the N*N float matrix, 0<seed<1, float
This function should initialize two matrixs with rand_float()
*/
float rand_float(float s){
	return 4*s*(1-s);
}
void matrix_gen(float *a,float *b,int N,float seed){
	float s=seed;
	for(int i=0;i<N*N;i++){
		s=rand_float(s);
		a[i]=s;
		s=rand_float(s);
		b[i]=s;
	}
}
```

### 1. 基准程序
```c
//基准矩阵乘法，a,b为输入矩阵的指针，c为输出矩阵的指针，N为矩阵的阶数
void matrix_multiply(float *a,float *b, float *c, int N){
   int i,j,k;
   for(i=0;i<N;i++){
      for(j=0;j<N;j++){
         float sum=0.0;
         for(k=0;k<N;k++){
            sum+=a[i*N+k]*b[k*N+j];    //矩阵元素访问
         }
         c[i*N+j]=sum;
      }
   }
}
```
调整矩阵规模，观察不同矩阵规模下串行程序执行时间的变化。

| N | 512 | 1024 | 2048 | 4096 |
|---|---|---|---|---|
|Trace|
|计算时间(s)|
> 表2-1 不同规模矩阵的串行计算时间

根据 表2-1 绘制 图2-1，其中 X轴 为 N ，Y轴 为矩阵规模为N时的计算时间。

> 图2-1 不同矩阵规模的计算时间

从图2-1中，可以得到的结论是

### 2.矩阵分块计算
