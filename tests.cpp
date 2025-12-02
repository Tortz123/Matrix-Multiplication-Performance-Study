#include <iostream>
#include <vector>
#include "matrix.h"
#include <Eigen/Dense>
#include <cstdint>
 
using Eigen::MatrixXd;

using namespace std;

void print_matrix(const vector<double>& mat, uint64_t N) {
    for (uint64_t i = 0; i < N; ++i) {
        for (uint64_t j = 0; j < N; ++j) {
            cout << mat[i * N + j] << " ";
        }
        cout << endl;
    }
}

// g++ -Wall -Wpedantic -std=c++17 -march=haswell -O3 -Ivectorclass -IEigen-5.0.0 -c matrix.cpp tests.cpp && g++ matrix.o tests.o && ./a.out
int main() {
    // can adjust the size of the matrices with N
    const uint64_t N = 4;

    // make_matrix_random:
    // vector<double> A = make_matrix_random(N);
    // vector<double> B = make_matrix_random(N);

    // make_matrix_incremental:
    // vector<double> A = make_matrix_incremental(N);
    // vector<double> B = make_matrix_incremental(N);

    // make_matrix_random_alternating:
    vector<double> A = make_matrix_random_alternating(N);
    vector<double> B = make_matrix_random_alternating(N);

    cout << "\n MATRIX A\n";
    print_matrix(A, N);
    cout << "\nMATRIX B\n";
    print_matrix(B, N);

    vector<double> out_basic;
    vector<double> out_eigen;
    vector<double> out_simd(N * N, 0.0);

    cout << "\nBASIC RESULT\n";
    matrix_mul_basic(A, B, out_basic, N);
    print_matrix(out_basic, N);

    cout << "\nEIGEN RESULT\n";
    matrix_mul_eigen(A, B, out_eigen, N);
    print_matrix(out_eigen, N);

    cout << "\nSIMD RESULT\n";
    matrix_mul_vc(A.data(), B.data(), out_simd.data(), N);
    print_matrix(out_simd, N);
}
