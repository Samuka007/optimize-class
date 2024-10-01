#ifndef BASELINE_HPP
#define BASELINE_HPP
/*
Input: a, b are the N*N float matrix, 0<seed<1, float
This function should initialize two matrixs with rand_float()
*/
inline float rand_float(float s){
	return 4*s*(1-s);
}
inline void matrix_gen(float *a,float *b, int N, float seed){
	float s=seed;
	for(int i=0;i<N*N;i++){
		s=rand_float(s);
		a[i]=s;
		s=rand_float(s);
		b[i]=s;
	}
}

//基准矩阵乘法，a,b为输入矩阵的指针，c为输出矩阵的指针，N为矩阵的阶数
void matrix_multiply(const float *a,const float *b, float *c, int N);
#endif