#include "boundary.h"

void initBoundary(double* __restrict A, int N) {
    const double tl = 10.0;
    const double tr = 20.0;
    const double br = 30.0;
    const double bl = 20.0;

    for (int i = 0; i < N; ++i) {
        A[i] = tl + (tr - tl) * static_cast<double>(i) / (N - 1);
    }

    for (int j = 0; j < N; ++j) {
        A[j * N + (N - 1)] = tr + (br - tr) * static_cast<double>(j) / (N - 1);
    }

    for (int i = 0; i < N; ++i) {
        A[(N - 1) * N + i] = bl + (br - bl) * static_cast<double>(i) / (N - 1);
    }

    for (int j = 0; j < N; ++j) {
        A[j * N] = tl + (bl - tl) * static_cast<double>(j) / (N - 1);
    }
}