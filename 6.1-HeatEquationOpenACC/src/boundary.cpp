#include <vector>

#include "boundary.h"

void initBoundary(std::vector<double>& A, int N) {
    // Значения в углах
    const double tl = 10.0; // top-left
    const double tr = 20.0; // top-right
    const double br = 30.0; // bottom-right
    const double bl = 20.0; // bottom-left

    // Верхняя сторона: от tl до tr
    for (int i = 0; i < N; ++i) {
        A[i] = tl + (tr - tl) * double(i) / (N - 1);
    }
    // Правая сторона: от tr до br
    for (int j = 0; j < N; ++j) {
        A[j * N + (N - 1)] = tr + (br - tr) * double(j) / (N - 1);
    }
    // Нижняя сторона: от bl до br
    for (int i = 0; i < N; ++i) {
        A[(N - 1) * N + i] = bl + (br - bl) * double(i) / (N - 1);
    }
    // Левая сторона: от tl до bl
    for (int j = 0; j < N; ++j) {
        A[j * N] = tl + (bl - tl) * double(j) / (N - 1);
    }
}