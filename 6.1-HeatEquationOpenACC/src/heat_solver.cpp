#include <iostream>
#include <vector>
#include <cmath>
#include <boost/program_options.hpp>

#include "heat_solver.h"

Options parseOptions(int argc, char** argv) {
    namespace po = boost::program_options;
    Options opt;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "show help message")
        ("size", po::value<int>(&opt.N)->default_value(128), "grid size N")
        ("tol", po::value<double>(&opt.tol)->default_value(1e-6), "tolerance")
        ("max-iter", po::value<long>(&opt.maxIter)->default_value(1000000), "max iterations");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        std::exit(0);
    }
    po::notify(vm);
    return opt;
}

void initInterior(std::vector<double>& A, int N) {
    // Обнуляем внутренние точки, границы задаются отдельно
    for (int j = 1; j < N - 1; ++j) {
        for (int i = 1; i < N - 1; ++i) {
            A[j * N + i] = 0.0;
        }
    }
}

void jacobiIteration(const std::vector<double>& A,
                     std::vector<double>& Anew,
                     int N,
                     double& maxError) {
    const int NM = N * N;
    double localErr = 0.0;

    #pragma acc data copyin(A[0:NM]) copyout(Anew[0:NM])
    {
        #pragma acc parallel loop collapse(2) reduction(max:localErr)
        for (int j = 1; j < N - 1; ++j) {
            for (int i = 1; i < N - 1; ++i) {
                int idx = j * N + i;
                Anew[idx] = 0.25 * (A[idx + 1] + A[idx - 1]
                                    + A[idx + N] + A[idx - N]);
                double diff = std::fabs(Anew[idx] - A[idx]);
                if (diff > localErr) localErr = diff;
            }
        }
    }
    maxError = localErr;
}

void swapGrids(std::vector<double>& A, std::vector<double>& Anew) {
    A.swap(Anew);
}

void printSummary(long iter, double maxError) {
    std::cout << "Iterations: " << iter
              << ", Max Error: " << maxError << std::endl;
}