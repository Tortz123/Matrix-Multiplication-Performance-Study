#include <iostream>
#include <vector>
#include "vectorclass.h"
#include <cassert>
#include <Eigen/Dense>
#include <random>
using Eigen::MatrixXd;

using namespace std;


// https://www.geeksforgeeks.org/dsa/c-program-multiply-two-matrices/
void matrix_mul_basic(const vector<double>& A, const vector<double>& B, vector<double>& output, uint64_t size) {
    output.assign(size * size, 0.0);

    for (uint64_t i = 0; i < size; ++i) {
        for (uint64_t j = 0; j < size; ++j) {
            double sum = 0.0;
            for (uint64_t k = 0; k < size; ++k) {
                sum += A[i * size + k] * B[k * size + j];
            }
            output[i * size + j] = sum;
        }
    }
}

// https://libeigen.gitlab.io/eigen/docs-nightly/index.html
void matrix_mul_eigen(const vector<double>& A, const vector<double>& B, vector<double>& output, uint64_t size) {
    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> matA(A.data(), size, size);
    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> matB(B.data(), size, size);
    MatrixXd matC = matA * matB;

    output.resize(size*size);
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(output.data(), size, size) = matC;
}

void matrix_mul_vc(const double* A, const double* B, double* output, uint64_t size) {
    assert(size % 4 == 0); 

    for (uint64_t i = 0; i < size * size; ++i)
        output[i] = 0.0;

    for (uint64_t i = 0; i < size; ++i) {

        for (uint64_t k = 0; k < size; ++k) {

            Vec4d a_vec = Vec4d(A[i*size + k]);

            for (uint64_t j = 0; j < size; j += 4) {

                Vec4d c_vec;
                c_vec.load(output + i*size + j);   // load 4 partial sums

                Vec4d b_vec;
                b_vec.load(B + k*size + j);   // load 4 B values

                
                c_vec = c_vec + a_vec * b_vec;

                c_vec.store(output + i*size + j);
            }
        }
    }
}


// https://stackoverflow.com/questions/27934180/math-random-equivalent-in-c
vector<double> make_matrix_random(uint64_t size) {
    vector<double> matrix(size * size);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(-1.0, 1.0);

    for (uint64_t i = 0; i < size * size; ++i) {
        matrix[i] = dist(gen);
    }
    return matrix;
}

vector<double> make_matrix_incremental(uint64_t size) {
    vector<double> matrix(size * size);
    for (uint64_t i = 0; i < size * size; ++i) {
        matrix[i] = static_cast<double>(i + 1);
    }
    return matrix;
}

// https://stackoverflow.com/questions/27934180/math-random-equivalent-in-c
vector<double> make_matrix_random_alternating(uint64_t size) {
    vector<double> matrix(size * size);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.1, 10.0); 

    for (uint64_t i = 0; i < size * size; ++i) {
        double val = dist(gen);
        if (i % 2 == 0) {
            matrix[i] = val;
        } else {
            matrix[i] = -val;
        }
    }
    return matrix;
}





