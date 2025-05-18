#include <iostream>
#include <chrono>
#include <cstdlib>
#include <fstream>

#include "heat_solver.h"
#include "boundary.h"

int main(int argc, char** argv) {
    auto opt = parseOptions(argc, argv);
    int N  = opt.N;
    int NM = N * N;

    double* A    = static_cast<double*>(
                     aligned_alloc(64, NM * sizeof(double)));
    double* Anew = static_cast<double*>(
                     aligned_alloc(64, NM * sizeof(double)));

    initBoundary(A,    N);
    initBoundary(Anew, N);
    initInterior(A,    N);
    initInterior(Anew, N);

    long   iter   = 0;
    double maxErr = 0.0;

    auto t_start = std::chrono::high_resolution_clock::now();

    #pragma acc data copy(A[0:NM]) copy(Anew[0:NM])
    {
        do {
            jacobiIteration(A, Anew, N, maxErr);
            swapGrids(A, Anew);
            ++iter;
            std::cout << "Iteration " << iter << ": maxError = " << maxErr << std::endl;
        } while (maxErr > opt.tol && iter < opt.maxIter);
    }

    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t_end - t_start;

    printSummary(iter, maxErr);
    std::cout << "Elapsed time: " << elapsed.count() << " sec\n";

    if (N == 10) {
        std::ofstream fout("matrix_10x10.csv");
        for (int j = 0; j < N; ++j) {
            for (int i = 0; i < N; ++i) {
                fout << A[j * N + i];
                if (i < N - 1) fout << ",";
            }
            fout << "\n";
        }
        std::cout << "Matrix saved to matrix_10x10.csv\n";
    }

    free(A);
    free(Anew);
    return 0;
}