#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <cstdint>

using namespace std;

void matrix_mul_basic(const vector<double>& A, const vector<double>& B, vector<double>& output, uint64_t N);

void matrix_mul_vc(const double* A, const double* B, double* output, uint64_t N);

void matrix_mul_eigen(const vector<double>& A, const vector<double>& B, vector<double>& output, uint64_t N);

vector<double> make_matrix_random(uint64_t N);

vector<double> make_matrix_incremental(uint64_t N);

vector<double> make_matrix_random_alternating(uint64_t N);

#endif // MATRIX_H
