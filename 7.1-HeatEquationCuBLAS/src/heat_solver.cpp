#include <cmath>
#include <iostream>
#include <boost/program_options.hpp>
#include <cublas_v2.h>
#include <openacc.h>

#include "heat_solver.h"

Options parseOptions(int argc, char** argv) {
    namespace po = boost::program_options;
    Options opt;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h",   "show help")
        ("size",     po::value<int>(&opt.N)->default_value(128))
        ("tol",      po::value<double>(&opt.tol)->default_value(1e-6))
        ("max-iter", po::value<long>(&opt.maxIter)->default_value(1000000));
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) { std::cout << desc << "\n"; std::exit(0); }
    po::notify(vm);
    return opt;
}

void initInterior(double* __restrict A, int N) {
    for (int j = 1; j < N - 1; ++j) {
        double* row = A + j * N;
        for (int i = 1; i < N - 1; ++i) {
            row[i] = 0.0;
        }
    }
}

void jacobiIteration(const double* __restrict A,
                     double* __restrict Anew,
                     int N,
                     double& maxError) {
    int NM = N * N;
    #pragma acc parallel loop collapse(2) present(A[0:NM], Anew[0:NM])
    for (int j = 1; j < N - 1; ++j) {
        for (int i = 1; i < N - 1; ++i) {
            int idx = j * N + i;
            double v = 0.25 * (A[idx-1] + A[idx+1] + A[idx+N] + A[idx-N]);
            Anew[idx] = v;
        }
    }
    double* diff = (double*)malloc(NM * sizeof(double));
    #pragma acc parallel loop collapse(2) create(diff[0:NM]) present(A[0:NM], Anew[0:NM])
    for (int j = 0; j < N; ++j) {
        for (int i = 0; i < N; ++i) {
            int idx = j * N + i;
            diff[idx] = fabs(Anew[idx] - A[idx]);
        }
    }
    int idxMax = 0;
    cublasIdamax(cublasHandle, NM, diff, 1, &idxMax);
    double maxDiff = 0.0;
    cublasGetVector(1, sizeof(double), diff + (idxMax - 1), 1, &maxDiff, 1);
    maxError = maxDiff;
    free(diff);
}

void printSummary(long iter, double maxError) {
    std::cout << "Iterations: " << iter
              << ", Max Error: " << maxError << "\n";
}