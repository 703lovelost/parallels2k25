#pragma once

#include <cublas_v2.h>

extern cublasHandle_t cublasHandle;

struct Options {
    int    N;
    long   maxIter;
    double tol;
};

Options parseOptions(int argc, char** argv);

void initInterior(double* __restrict A, int N);

void jacobiIteration(const double* __restrict A,
                     double* __restrict Anew,
                     int N,
                     double& maxError);

inline void swapGrids(double*& A, double*& Anew) {
    double* tmp = A; A = Anew; Anew = tmp;
}

void printSummary(long iter, double maxError);