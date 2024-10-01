#include <iostream>
#include <span>

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

auto get_arguments(int argc, char** argv) {
    // Matrix_mul N seed
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <seed>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int N = std::stoi(argv[1]);
    float seed = std::stof(argv[2]);

    return std::make_tuple(N, seed);
}

namespace Matrix2D {
    float get_trace(float* matrix, int N) {
        float sum {0.0};
        for (std::size_t i {0}; i < N; ++i) {
            sum += matrix[i*N + i];
        }
        return sum;
    }

}


int main(int argc, char** argv) {
    auto [N, seed] = get_arguments(argc, argv);
    float *a = new float[N*N];
    float *b = new float[N*N];
    float *c = new float[N*N];
    matrix_gen(a, b, N, seed);
    matrix_multiply(a, b, c, N);
    std::cout << "Trace of matrix c: " << Matrix2D::get_trace(c, N) << std::endl;
    return 0;
}
