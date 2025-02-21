#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <malloc.h>
#include <inttypes.h>

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

/*
* Functions for parallel processing.
*/

void dgemv_parallel(double *A, double *v, double *u, long matrix_size, long num_threads)
{   
    #pragma omp parallel num_threads(num_threads)
    {
        int thread_num = omp_get_thread_num();

        int items_per_thread = matrix_size / num_threads;
        int lb = thread_num * items_per_thread;
        int ub = (thread_num == num_threads - 1) ? (matrix_size - 1) : (lb + items_per_thread - 1);
        
        for (int i = lb; i <= ub; i++) {
            for (int j = 0; j < matrix_size; j++) {
                u[i] += A[i * matrix_size + j] * v[j];
            }
        }
    }
}

double run_parallel(long matrix_size, long num_threads) {
    double *A;  // Matrix A
    double *v;  // Vector v
    double *u;  // Vector u = Av
    double t;   // For time measurement

    A = malloc(sizeof(double) * matrix_size * matrix_size);
    v = malloc(sizeof(double) * matrix_size);
    u = calloc(matrix_size, sizeof(double));

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            A[i * matrix_size + j] = i + j;
        }
    }

    for (int i = 0; i < matrix_size; i++) {
        v[i] = matrix_size - i;
    }

    t = cpuSecond();
    dgemv_parallel(A, v, u, matrix_size, num_threads);
    t = cpuSecond() - t;

    free(A);
    free(v);
    free(u);

    return t;
}

/*
* Functions for serial processing.
*/

void dgemv_serial(double *A, double *v, double *u, long matrix_size)
{
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            u[i] += A[i * matrix_size+ j] * v[j];
        }
    }
}

double run_serial(long matrix_size)
{
    double *A;  // Matrix A
    double *v;  // Vector v
    double *u;  // Vector u = Av
    double t;   // For time measurement

    A = malloc(sizeof(double) * matrix_size * matrix_size);
    v = malloc(sizeof(double) * matrix_size);
    u = calloc(matrix_size, sizeof(double));

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            A[i * matrix_size + j] = i + j;
        }
    }

    for (int i = 0; i < matrix_size; i++) {
        v[i] = matrix_size - i;
    }

    t = cpuSecond();
    dgemv_serial(A, v, u, matrix_size);
    t = cpuSecond() - t;

    free(A);
    free(v);
    free(u);

    return t;
}

int main(int argc, char **argv)
{
    long num_threads, matrix_size;
    double tserial, tparallel;

    if (argc < 3) {
        printf("Not enough arguments.\n");
        return 1;
    }

    num_threads = strtol(argv[1], NULL, 10);
    matrix_size = strtol(argv[2], NULL, 10);

    // Default settings in case of wrong input.
    // Based on strtol() conversion behavior.
    if (matrix_size == 0) {
        matrix_size = 20000;
    }

    printf("Number of threads used: %ld\n", num_threads);
    printf("The size of the matrix: %ld x %ld\n", matrix_size, matrix_size);

    tserial = run_serial(matrix_size);
    printf("Elapsed time (serial): %.12f sec.\n", tserial);

    if (num_threads > 1) {
        tparallel = run_parallel(matrix_size, num_threads);
        printf("Elapsed time (parallel): %.12f sec.\n", tparallel);
        printf("Speedup: %.2f\n", tserial / tparallel);
    }

    return 0;
}