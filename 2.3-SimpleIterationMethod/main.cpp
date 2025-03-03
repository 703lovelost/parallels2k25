#include <iostream>
#include <numeric>
#include <iomanip>
#include <vector>
#include <chrono>
#include <omp.h>

void simple_iteration(std::vector<std::vector<double>> A, std::vector<double> v,
                    std::vector<double> u, int matrix_size, int num_threads)
{
    #pragma omp parallel num_threads(num_threads)
    {
        int thread_num = omp_get_thread_num();
        double tau = 0.01;
        double eps = 0.0001;
        double u_mult_fold = std::accumulate(std::begin(u), std::end(u), 1.0, std::multiplies<double>());

        int items_per_thread = matrix_size / num_threads;
        int lb = thread_num * items_per_thread;
        int ub = (thread_num == num_threads - 1) ? (matrix_size - 1) : (lb + items_per_thread - 1);

        for (;;) {
            break;
            // std::vector<double> Av_minus_u;
            // std::vector<double> 

            // for (int i = lb; i <= ub; i++) {
            //     for (int j = 0; j < matrix_size; j++) {
            //         Av_minus_u[i] = (A[i][j] * v[j]) - u[j];
            //     }
            // }
          
            // double Av_minux_u_mult_fold = std::accumulate(std::begin(Av_minus_u), std::end(Av_minus_u), 1.0, std::multiplies<double>());
    
            // if (Av_minux_u_mult_fold / u_mult_fold < eps) {

            // }
            // v[j] -= (tau * Av_minus_u);
        }
        

    }
}

double run(int matrix_size, int num_threads) {
    std::vector<std::vector<double>> A(matrix_size,
                                        std::vector<double>(matrix_size, 0));   // Matrix A
    std::vector<double> v;                                                      // Vector v
    std::vector<double> u;                                                      // Vector u
    double t;                                                                   // For time measurement

    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            if (i == j) {
                A[i][j] = 2.0;
            }
            else {
                A[i][j] = 1.0;
            }
        }
    }

    for (int i = 0; i < matrix_size; i++) {
        v[i] = 0;
    }

    for (int i = 0; i < matrix_size; i++) {
        u[i] = matrix_size + 1.0;
    }

    const auto start{std::chrono::steady_clock::now()};
    simple_iteration(A, v, u, matrix_size, num_threads);
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    t = elapsed_seconds.count();

    return t;
}

int main(int argc, char *argv[]) {
    int num_threads, matrix_size;
    double tserial, tparallel;

    if (argc < 3) {
        printf("Not enough arguments.\n");
        return 1;
    }

    num_threads = atoi(argv[1]);
    matrix_size = atoi(argv[2]);

    // Default settings in case of wrong input.
    // Based on atoi() conversion behavior.
    if (matrix_size == 0) {
        matrix_size = 20000;
    }

    std::cout << "Number of threads used: " << num_threads << "\n";
    std::cout << "The size of the matrix: " << matrix_size << " x " << matrix_size << "\n";

    tserial = run(matrix_size, 1);
    std::cout << "Elapsed time (serial): " << std::setprecision(12) << tserial << "\n";

    if (num_threads > 1) {
        tparallel = run(matrix_size, num_threads);
        std::cout << "Elapsed time (parallel): " << std::setprecision(12) << tparallel << "\n";
        std::cout << "Elapsed time (parallel): " << std::setprecision(2) << tserial / tparallel << "\n";  
    }

    return 0;
}