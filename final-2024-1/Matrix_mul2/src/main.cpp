#include <cmath>
#include <string>
#include <tuple>
#include <vector>
#include "allocator.hpp"

#ifdef NDEBUG
#include <cassert>
#include <chrono>
#include <Eigen/Dense>
#endif

#ifdef SIMD_MULT
#include <immintrin.h>
#endif

#ifdef FAST_IO
#include <fast_io.h>
using fast_io::io::print;
#else

#include <iostream>
void print(auto&&... args) {
    (std::cout << ... << args);
}

#endif

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

void matrix_gen_test(float* a, float* b, int N) {
    for (int i {0}; i < N * N; ++i) {
        a[i] = i / 10.0f;
        b[i] = i / 10.0f;
    }
}

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


/**
    cache line (l1 cache) and l2 cache is per core
*/
constexpr std::size_t cache_line_size = 
    #ifdef __cpp_lib_hardware_interference_size
    #include <new>
    std::max(
        std::hardware_constructive_interference_size,
        std::hardware_destructive_interference_size
    );
    #elif defined (L1_CACHE)
    L1_CACHE;
    #else
    64 /* KB */ << 10; // x64 platform univesal l1 cache size
    #endif

constexpr std::size_t l2_cache_size =
    #ifdef L2_CACHE
    L2_CACHE;
    #else
    256 /* KB */ << 10; // l2 cache on e5-2666v3
    #endif

auto get_arguments(int argc, char** argv) {
    // Matrix_mul N seed
    if (argc != 3) {
        #ifdef NDEBUG
        return std::make_tuple(
            512,
            // 1024,
            // 2048,
            // 4096,
            // 8192,
            0.0355f
            // 0.6f
        );
        #else
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <seed>" << std::endl;
        exit(EXIT_FAILURE);
        #endif
    }

    int N = std::stoi(argv[1]);
    float seed = std::stof(argv[2]);

    return std::make_tuple(N, seed);
}

inline void invoke_and_show_result(std::invocable auto&& func) {
    #ifdef NDEBUG
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    // func();
    auto end = std::chrono::high_resolution_clock::now();
    print("Result: ", result, "\n");
    print("Time: ", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), "ms\n");
    #else
    print(func());
    #endif
}

namespace Matrix2D {
    float get_trace(const float* matrix, int N) {
        float sum {0.0};
        for (std::size_t i {0}; i < N; ++i) {
            sum += matrix[i*N + i];
        }
        return sum;
    }

    inline void simd_mul(const float* lhs, const float* rhs, float* result, int N) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; j += 8) {
                for (int k = 0; k < N; ++k) {
                    __m256 a = _mm256_set1_ps(lhs[i * N + k]);
                    __m256 b = _mm256_load_ps(rhs + k * N + j);
                    __m256 c = _mm256_load_ps(result + i * N + j);
                    // c = _mm256_fmadd_ps(a, b, c); // the old machine does not support fma
                    c = _mm256_add_ps(c, _mm256_mul_ps(a, b));
                    _mm256_store_ps(result + i * N + j, c);
                }
            }
        }
    }

    inline void simd_with_block(const float* lhs, const float* rhs, float* result, int N) {
        // 可以将矩阵分解为m×m的矩阵小块，每次完成一对小块的计算，以提高Cache的命中率。
        const int small_block_size = 
        // std::sqrt(
        //     // cache_line_size // Time: 11701ms
        //     l2_cache_size // Time: 6199ms
        //     / (2 * sizeof(float))
        // );
        256;
        
        // print("small_block_size: ", small_block_size, "\n");

        for (int i = 0; i < N; i += small_block_size) {
            for (int j = 0; j < N; j += small_block_size) {
                for (int k = 0; k < N; k += small_block_size) {

                    for (int ii = i; ii < std::min(i + small_block_size, N); ++ii) {
                        for (int jj = j; jj < std::min(j + small_block_size, N); jj+=8) {
                            for (int kk = k; kk < std::min(k + small_block_size, N); ++kk) {
                                __m256 a = _mm256_set1_ps(lhs[ii * N + kk]);
                                __m256 b = _mm256_load_ps(rhs + kk * N + jj);
                                __m256 c = _mm256_load_ps(result + ii * N + jj);
                                c = _mm256_add_ps(c, _mm256_mul_ps(a, b));
                                _mm256_store_ps(result + ii * N + jj, c);
                            }
                        }
                    }

                }
            }
        }
    }

    class SquareMatrix {
    public:
        std::vector<float, Allocator32<float>> data;
        int N;

        SquareMatrix(int N) : N(N), data(N*N) {
            // set all elements to 0
            std::fill(data.begin(), data.end(), 0.0);
        }

        inline float& operator()(int i, int j) {
            return data[i*N + j];
        }

        inline const float& operator()(int i, int j) const {
            return data[i*N + j];
        }

        SquareMatrix operator*(const SquareMatrix& other) const {
            #ifdef NDEBUG
            assert(this->N == other.N);
            #endif

            SquareMatrix result {this->N};
            #ifdef MULTI_THREAD
            #elif defined(SIMD_MULT)
            simd_with_block(this->data.data(), other.data.data(), result.data.data(), this->N);
            // simd_mul(this->data.data(), other.data.data(), result.data.data(), this->N);
            #elif defined(PARTIAL_MULT) // 矩阵分块

            // 可以将矩阵分解为m×m的矩阵小块，每次完成一对小块的计算，以提高Cache的命中率。
            // n * n * 2 * sizeof(float) = l2_cache_size, where small_block_size = n
            const int small_block_size = std::sqrt(
                // cache_line_size // Time: 11701ms
                l2_cache_size // Time: 6199ms
                / (2 * sizeof(float)));

            print("small_block_size: ", small_block_size, "\n");

            for (int i = 0; i < N; i += small_block_size) {
                for (int j = 0; j < N; j += small_block_size) {
                    for (int k = 0; k < N; k += small_block_size) {
                        for (int ii = i; ii < std::min(i + small_block_size, N); ++ii) {
                            for (int jj = j; jj < std::min(j + small_block_size, N); ++jj) {
                                float sum = 0.0;
                                for (int kk = k; kk < std::min(k + small_block_size, N); ++kk) {
                                    sum += this->data[ii * N + kk] * other.data[kk * N + jj];
                                }
                                result.data[ii * N + jj] += sum;
                            }
                        }
                    }
                }
            }

            #else
            // 基准矩阵乘法：Time: 3398ms
            matrix_multiply(this->data.data(), other.data.data(), result.data.data(), this->N);

            #endif
            return result;
        }

        float trace() const {
            return get_trace(data.data(), N);
        }

        friend std::ostream& operator<<(std::ostream& os, const SquareMatrix& matrix) {
            for (int i = 0; i < matrix.N; ++i) {
                for (int j = 0; j < matrix.N; ++j) {
                    os << matrix(i, j) << ' ';
                }
                os << '\n';
            }
            return os;
        }
        
    };

    auto get_pair_of_matrices(int N, float seed) {
        SquareMatrix a {N};
        SquareMatrix b {N};
        matrix_gen(a.data.data(), b.data.data(), N, seed);
        // matrix_gen_test(a.data.data(), b.data.data(), N);
        return std::make_pair(a, b);
    }
}


int main(int argc, char** argv) {
    auto [N, seed] = get_arguments(argc, argv);

    auto [a2, b2] = Matrix2D::get_pair_of_matrices(N, seed);

    #ifndef FAST_IO
    std::ios_base::sync_with_stdio(false);
    #endif

    #ifdef NDEBUG
        // validate using Eigen
        Eigen::MatrixXf a {N, N};
        Eigen::MatrixXf b {N, N};
        matrix_gen(a.data(), b.data(), N, seed);
        print("Eigen:\n");
        invoke_and_show_result([&] {
            auto c = a * b;
            return c.eval().trace();
        });
        #ifdef MULTI_THREAD
            print("\nMulti-threaded matrix multiplication\n");
        #elif defined(PARTIAL_MULT)
            print("\nPartial matrix multiplication\n");
        #elif defined(SIMD_MULT)
            print("\nSIMD matrix multiplication\n");
        #else
            print("\nBaseline matrix multiplication\n");
        #endif
    #endif

    invoke_and_show_result([&] {
        return (a2 * b2).trace();
    });


    return 0;
}
