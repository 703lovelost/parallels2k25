#include <cmath>
#include <algorithm>
#include <iostream>
#include <boost/program_options.hpp>

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
    if (vm.count("help")) { std::cout << desc << std::endl; std::exit(0); }
    po::notify(vm);
    return opt;
}

void initInterior(double* __restrict A, int N) {
    for (int j = 1; j < N-1; ++j) {
        double* row = A + j * N;
        for (int i = 1; i < N-1; ++i) {
            row[i] = 0.0;
        }
    }
}

void jacobiIteration(const double* __restrict A,
                     double* __restrict Anew,
                     int N,
                     double& maxError)
{
    int NM = N * N;
    double localErr = 0.0;

    #pragma acc parallel loop collapse(2) present(A[0:NM], Anew[0:NM]) reduction(max:localErr)
    for (int j = 1; j < N-1; ++j) {
        const double* row      = A    + j * N;
        const double* row_above = row  - N;
        const double* row_below = row  + N;
        double*       row_new   = Anew + j * N;

        for (int i = 1; i < N-1; ++i) {
            double v = 0.25 * ( row[i-1] + row[i+1]
                              + row_above[i] + row_below[i] );
            row_new[i] = v;
            double diff = std::fabs(v - row[i]);
            if (diff > localErr) localErr = diff;
        }
    }

    maxError = localErr;
}

void printSummary(long iter, double maxError) {
    std::cout << "Iterations: " << iter
              << ", Max Error: " << maxError << std::endl;
}