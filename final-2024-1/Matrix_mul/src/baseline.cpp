#include "baseline.hpp"

//基准矩阵乘法，a,b为输入矩阵的指针，c为输出矩阵的指针，N为矩阵的阶数
void matrix_multiply(const float *a, const float *b, float *c, int N){
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