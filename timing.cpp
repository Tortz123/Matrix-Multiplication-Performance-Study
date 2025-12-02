// Enable clock_gettime: https://stackoverflow.com/a/40515669
#define _POSIX_C_SOURCE 199309L
#include <iostream>
#include <vector>
#include "matrix.h"
#include <cstdint>
#include <assert.h>
#include <time.h>

using namespace std;

#define LENGTH        40000000

#define TIMING_RESULT(DESCR, CODE) do { \
    struct timespec start, end; \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); \
    CODE; \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end); \
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; \
    printf("%10s took %7.1f ms\n", descr, elapsed * 1000); \
} while(0)


void speed_test_random(const char* descr, void (*func)(const vector<double>&, const vector<double>&, vector<double>&, uint64_t), vector<double> (*make_matrix)(uint64_t),uint64_t N) {
    vector<double> A = make_matrix_random(N);
    vector<double> B = make_matrix_random(N);
    vector<double> output;

    TIMING_RESULT(descr, func(A, B, output, N));
}

// for SIMD implementation
void speed_test_random(const char* descr, void (*func)(const double*, const double*, double*, uint64_t), vector<double> (*make_matrix)(uint64_t), uint64_t N) {
    vector<double> A_vec = make_matrix_random(N);
    vector<double> B_vec = make_matrix_random(N);
    vector<double> output_vec(N * N, 0.0);

    const double* A = A_vec.data();
    const double* B = B_vec.data();
    double* output = output_vec.data();

    TIMING_RESULT(descr, func(A, B, output, N));
}


void speed_test_incremental(const char* descr, void (*func)(const vector<double>&, const vector<double>&, vector<double>&, uint64_t), vector<double> (*make_matrix)(uint64_t),uint64_t N) {
    vector<double> A = make_matrix_incremental(N);
    vector<double> B = make_matrix_incremental(N);
    vector<double> output;

    TIMING_RESULT(descr, func(A, B, output, N));
}

// for SIMD implementation
void speed_test_incremental(const char* descr, void (*func)(const double*, const double*, double*, uint64_t), vector<double> (*make_matrix)(uint64_t), uint64_t N) {
    vector<double> A_vec = make_matrix_incremental(N);
    vector<double> B_vec = make_matrix_incremental(N);
    vector<double> output_vec(N * N, 0.0);

    const double* A = A_vec.data();
    const double* B = B_vec.data();
    double* output = output_vec.data();

    TIMING_RESULT(descr, func(A, B, output, N));
}

void speed_test_random_alternating(const char* descr, void (*func)(const vector<double>&, const vector<double>&, vector<double>&, uint64_t), vector<double> (*make_matrix)(uint64_t),uint64_t N) {
    vector<double> A = make_matrix_random_alternating(N);
    vector<double> B = make_matrix_random_alternating(N);
    vector<double> output;

    TIMING_RESULT(descr, func(A, B, output, N));
}

// for SIMD implementation
void speed_test_random_alternating(const char* descr, void (*func)(const double*, const double*, double*, uint64_t), vector<double> (*make_matrix)(uint64_t), uint64_t N) {
    vector<double> A_vec = make_matrix_random_alternating(N);
    vector<double> B_vec = make_matrix_random_alternating(N);
    vector<double> output_vec(N * N, 0.0);

    const double* A = A_vec.data();
    const double* B = B_vec.data();
    double* output = output_vec.data();

    TIMING_RESULT(descr, func(A, B, output, N));
}


// g++ -Wall -Wpedantic -std=c++17 -march=haswell -O3 -Ivectorclass -IEigen-5.0.0 -c matrix.cpp timing.cpp && g++ matrix.o timing.o && ./a.out
int main() {
    const int N = 128;

    // warmup
    speed_test_random("warmup", matrix_mul_basic, make_matrix_random, N);
    speed_test_random("warmup", matrix_mul_basic, make_matrix_random, N);
    speed_test_random("warmup", matrix_mul_basic, make_matrix_random, N);
    printf("--------------------------------------------\n");


    printf("         make_matrix_random Timing        \n");
    printf("                Size: %4dx%-4d               \n", N, N);
    printf("--------------------------------------------\n");


    
    speed_test_random("Basic", matrix_mul_basic, make_matrix_random, N);
    speed_test_random("Eigen", matrix_mul_eigen, make_matrix_random, N);
    speed_test_random("SIMD", matrix_mul_vc, make_matrix_random, N);
    printf("----------------------------------------------\n");

    
    printf("         make_matrix_incremental Timing        \n");
    printf("                Size: %4dx%-4d               \n", N, N);
    printf("--------------------------------------------\n");

    
    speed_test_random("Basic", matrix_mul_basic, make_matrix_incremental, N);
    speed_test_random("Eigen", matrix_mul_eigen, make_matrix_incremental, N);
    speed_test_random("SIMD", matrix_mul_vc, make_matrix_incremental, N);
    printf("----------------------------------------------\n");



    printf(" make_matrix_random_alternating Timing        \n");
    printf("                Size: %4dx%-4d               \n", N, N);
    printf("--------------------------------------------\n");

    
    speed_test_random("Basic", matrix_mul_basic, make_matrix_random_alternating, N);
    speed_test_random("Eigen", matrix_mul_eigen, make_matrix_random_alternating, N);
    speed_test_random("SIMD", matrix_mul_vc, make_matrix_random_alternating, N);
    printf("----------------------------------------------\n");


    return 0;
}