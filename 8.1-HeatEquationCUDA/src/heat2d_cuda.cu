#include <iostream>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <boost/program_options.hpp>
#include <cub/cub.cuh>
#include <cuda_runtime.h>
#include "boundary.h"

struct Options { int N; long maxIter; double tol; };

Options parseOptions(int argc, char** argv){
    namespace po = boost::program_options;
    Options opt;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h","show help")
        ("size",     po::value<int>(&opt.N)->default_value(128))
        ("tol",      po::value<double>(&opt.tol)->default_value(1e-6))
        ("max-iter", po::value<long>(&opt.maxIter)->default_value(1000000));
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if (vm.count("help")) {
        std::cout << desc << "\n"; std::exit(0);
    }
    po::notify(vm);
    return opt;
}

void printSummary(long iter, double maxError){
    std::cout << "Iterations: " << iter << ", Max Error: " << maxError << "\n";
}

__global__ void jacobiKernel(const double* A, double* Anew, double* diff, int N){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;

    if (i > 0 && i < N-1 && j > 0 && j < N-1){
        int idx = j*N + i;
        double v = 0.25 * (A[idx-1] + A[idx+1] + A[idx+N] + A[idx-N]);
        Anew[idx] = v;
        diff[idx] = fabs(v - A[idx]);
    }
}

int main(int argc, char** argv){
    Options opt = parseOptions(argc, argv);
    int N = opt.N;
    int NM = N*N;

    int device_id = 3;
    cudaError_t err = cudaSetDevice(device_id);
    if (err != cudaSuccess) {
        std::cerr << "Couldn't set up GPU " << device_id<<": " << cudaGetErrorString(err) << "\n";
        std::exit(-1);
    }

    double* hA    = (double*)malloc(NM*sizeof(double));
    double* hAnew = (double*)malloc(NM*sizeof(double));
    initBoundary(hA, N);
    initBoundary(hAnew, N);
    for (int j = 1; j < N-1; ++j)
        for (int i = 1; i < N-1; ++i) {
            hA[j * N + i] = 0.0;
            hAnew[j * N + i] = 0.0;
        }

    double *dA, *dAnew, *dDiff, *dMaxErr;
    cudaMalloc(&dA,    NM*sizeof(double));
    cudaMalloc(&dAnew, NM*sizeof(double));
    cudaMalloc(&dDiff, NM*sizeof(double));
    cudaMalloc(&dMaxErr, sizeof(double));
    cudaMemcpy(dA,    hA,    NM*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(dAnew, hAnew, NM*sizeof(double), cudaMemcpyHostToDevice);

    void* dTempStorage = nullptr;
    size_t tempStorageBytes = 0;
    cub::DeviceReduce::Max(dTempStorage, tempStorageBytes, dDiff, dMaxErr, NM);
    cudaMalloc(&dTempStorage, tempStorageBytes);

    double* hMaxErrHost = nullptr;
    cudaMallocHost(&hMaxErrHost, sizeof(double));

    cudaStream_t stream;
    cudaStreamCreate(&stream);

    cudaGraph_t graphA, graphB;
    cudaGraphExec_t graphAExec, graphBExec;

    // Graph A (dA -> dAnew)
    {
        double* capA = dA;
        double* capAnew = dAnew;
        cudaStreamBeginCapture(stream, cudaStreamCaptureModeGlobal);
        jacobiKernel<<<dim3((N+15)/16, (N+15)/16), dim3(16,16), 0, stream>>>(capA, capAnew, dDiff, N);
        cub::DeviceReduce::Max(dTempStorage, tempStorageBytes, dDiff, dMaxErr, NM, stream);
        cudaMemcpyAsync(hMaxErrHost, dMaxErr, sizeof(double), cudaMemcpyDeviceToHost, stream);
        cudaStreamEndCapture(stream, &graphA);
        cudaGraphInstantiate(&graphAExec, graphA, nullptr, nullptr, 0);
    }
    // Graph B (dAnew -> dA)
    {
        double* capA = dAnew;
        double* capAnew = dA;
        cudaStreamBeginCapture(stream, cudaStreamCaptureModeGlobal);
        jacobiKernel<<<dim3((N+15)/16, (N+15)/16), dim3(16,16), 0, stream>>>(capA, capAnew, dDiff, N);
        cub::DeviceReduce::Max(dTempStorage, tempStorageBytes, dDiff, dMaxErr, NM, stream);
        cudaMemcpyAsync(hMaxErrHost, dMaxErr, sizeof(double), cudaMemcpyDeviceToHost, stream);
        cudaStreamEndCapture(stream, &graphB);
        cudaGraphInstantiate(&graphBExec, graphB, nullptr, nullptr, 0);
    }

    long iter = 0;
    double maxErr = 1e9;
    auto t_start = std::chrono::high_resolution_clock::now();
    while (maxErr > opt.tol && iter < opt.maxIter) {
        ++iter;
        if (iter % 2 == 1) {
            cudaGraphLaunch(graphAExec, stream);
        } else {
            cudaGraphLaunch(graphBExec, stream);
        }
        cudaStreamSynchronize(stream);
        maxErr = *hMaxErrHost;
        std::cout << "Iteration " << iter << ": maxError = " << maxErr << "\n";
    }
    auto t_end = std::chrono::high_resolution_clock::now();

    cudaDeviceSynchronize();
    printSummary(iter, maxErr);
    std::chrono::duration<double> elapsed = t_end - t_start;
    std::cout << "Elapsed time: " << elapsed.count() << " sec\n";

    if (N == 10) {
        double* dResult = (iter % 2 == 0) ? dA : dAnew;
        cudaMemcpy(hA, dResult, NM*sizeof(double), cudaMemcpyDeviceToHost);
        std::ofstream fout("matrix_10x10.csv");
        for (int j = 0; j < N; ++j) {
            for (int i = 0; i < N; ++i) {
                fout << hA[j * N + i] << (i < N-1 ? "," : "");
            }
            fout << "\n";
        }
        std::cout << "Matrix saved to matrix_10x10.csv\n";
    }

    cudaGraphExecDestroy(graphAExec);
    cudaGraphExecDestroy(graphBExec);
    cudaGraphDestroy(graphA);
    cudaGraphDestroy(graphB);
    cudaStreamDestroy(stream);

    cudaFree(dA);
    cudaFree(dAnew);
    cudaFree(dDiff);
    cudaFree(dMaxErr);
    cudaFree(dTempStorage);
    cudaFreeHost(hMaxErrHost);
    free(hA);
    free(hAnew);

    return 0;
}