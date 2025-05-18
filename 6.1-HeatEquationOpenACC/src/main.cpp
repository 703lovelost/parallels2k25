#include <iostream>
#include <vector>
#include <chrono>

#include "heat_solver.h"
#include "boundary.h"

int main(int argc, char** argv) {
    // Чтение параметров: N, tol, maxIter
    auto opt = parseOptions(argc, argv);
    int N = opt.N;

    // Выделение памяти для текущей и новой сетки
    std::vector<double> A(N * N);
    std::vector<double> Anew(N * N);

    // Инициализация: граничные условия и обнуление внутренней области
    initBoundary(A, N);
    initBoundary(Anew, N);
    initInterior(A, N);
    initInterior(Anew, N);

    long iter = 0;
    double maxError = 0.0;

    // Засекаем время вычислений
    auto t_start = std::chrono::high_resolution_clock::now();

    // Основной цикл метода Якоби
    do {
        jacobiIteration(A, Anew, N, maxError);
        swapGrids(A, Anew);
        ++iter;
        std::cout << "Iter " << iter << ": maxError = " << maxError << std::endl;
    } while (maxError > opt.tol && iter < opt.maxIter);

    // Останавливаем таймер
    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t_end - t_start;

    // Вывод результатов
    printSummary(iter, maxError);
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
