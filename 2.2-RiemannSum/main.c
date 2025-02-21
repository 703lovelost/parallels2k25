#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define NSTEPS 40000000

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

double func(double x)
{
    return exp(-x * x);
}

double integrate_omp(double (*func)(double), double a, double b, long num_threads)
{
    double h = (b - a) / NSTEPS;
    double sum = 0.0;

    #pragma omp parallel num_threads(num_threads)
    {
        int threadid = omp_get_thread_num();

        int items_per_thread = NSTEPS / num_threads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == num_threads - 1) ? (NSTEPS - 1) : (lb + items_per_thread - 1);

        double sumloc = 0.0;
        for (int i = lb; i <= ub; i++) {
            sumloc += func(a + h * (i + 0.5));
        }

        #pragma omp atomic
            sum += sumloc;
    }

    sum *= h;

    return sum;
}

double run(double a, double b, long num_threads)
{
    const double PI = 3.14159265358979323846;
    double t; // For time measurement
    double result;

    t = cpuSecond();
    result = integrate_omp(func, a, b, num_threads);
    t = cpuSecond() - t;

    printf("Result (%ld threads): %.12f; error %.12f\n", num_threads, result, fabs(result - sqrt(PI)));
    return t;
}

int main(int argc, char **argv)
{
    double a, b;
    double tserial, tparallel;
    long num_threads;

    if (argc < 4) {
        printf("Not enough arguments.\n");
        return 1;
    }

    a = strtod(argv[1], NULL);
    b = strtod(argv[2], NULL);
    num_threads = strtol(argv[3], NULL, 10);

    printf("Integration f(x) on [%.12f, %.12f], nsteps = %d\n", a, b, NSTEPS);
    tserial = run(a, b, 1);
    printf("Elapsed time (serial): %.12f sec.\n", tserial);

    if (num_threads > 1) {
        tparallel = run(a, b, num_threads);
        printf("Elapsed time (parallel): %.12f sec.\n", tparallel);
        printf("Speedup: %.2f\n", tserial / tparallel);
    }
    
    return 0;
}