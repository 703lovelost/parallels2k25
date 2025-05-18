#pragma once
#include <vector>

// Устанавливает граничные условия линейной интерполяцией
// A: вектор размера N*N, в котором хранятся значения на сетке
// N: размер стороны квадратной сетки
void initBoundary(std::vector<double>& A, int N);