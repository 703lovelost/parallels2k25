#include <iostream>
#include <numeric>
#include <iomanip>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cmath>

#define TAU 0.01
#define EPS 0.0001

double euclid_norm(std::vector<double> vector, int N) {
    double euclid = 0.0;
    
    for (int i = 0; i < N; i++) {
        euclid += pow(vector[i], 2.0);
    }

    return sqrt(euclid);
}

std::vector<double> simple_iteration(std::vector<std::vector<double>>& A,
                                    std::vector<double>& v,
                                    std::vector<double>& u,
                                    int matrix_size,
                                    int num_threads,
                                    const std::string& schedule_type)
{
    std::vector<double> Av_minus_u(matrix_size, 0.0);
    double Av_minus_u_euclid;
    double u_euclid;

    u_euclid = euclid_norm(u, matrix_size);
    // To avoid loop end miscalculation.
    Av_minus_u_euclid = u_euclid;

    #pragma omp parallel num_threads(num_threads)
    {

    do {
        if (schedule_type == "dynamic") {
            #pragma omp for schedule(dynamic, matrix_size/num_threads)
            for (int i = 0; i < matrix_size; i++) {
                double Av = 0.0;
                for (int j = 0; j < matrix_size; j++) {
                    Av += A[i][j] * v[j];
                }
                Av_minus_u[i] += Av - u[i];
            }
        }
        else {
            #pragma omp for schedule(static, matrix_size/num_threads)
            for (int i = 0; i < matrix_size; i++) {
                double Av = 0.0;
                for (int j = 0; j < matrix_size; j++) {
                    Av += A[i][j] * v[j];
                }
                Av_minus_u[i] += Av - u[i];
            }
        }

        #pragma omp barrier

        #pragma omp master
        Av_minus_u_euclid = euclid_norm(Av_minus_u, matrix_size);

        if (schedule_type == "dynamic") {
            #pragma omp for schedule(dynamic, matrix_size/num_threads)
            for (int i = 0; i < matrix_size; i++) {
                v[i] -= TAU * Av_minus_u[i];
                Av_minus_u[i] = 0.0;
            }
        }
        else {
            #pragma omp for schedule(static, matrix_size/num_threads)
            for (int i = 0; i < matrix_size; i++) {
                v[i] -= TAU * Av_minus_u[i];
                Av_minus_u[i] = 0.0;
            }
        }

    } while (Av_minus_u_euclid / u_euclid > EPS);

    }

    return v;
}

double run(int matrix_size, int num_threads, const std::string& schedule_type) {
    std::vector<std::vector<double>> A(matrix_size,
                                        std::vector<double>(matrix_size, 1.0));               // Matrix A
    std::vector<double> v(matrix_size, 0.0);                                                  // Vector v
    std::vector<double> u(matrix_size, matrix_size + 1);                                      // Vector u

    for (int i = 0; i < matrix_size; i++) {
        A[i][i] = 2.0;
    }

    const auto start{std::chrono::steady_clock::now()};
    v = simple_iteration(A, v, u, matrix_size, num_threads, schedule_type);
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    return elapsed_seconds.count();
}

int main(int argc, char *argv[]) {
    int num_threads, matrix_size;
    double tserial, tparallel;

    if (argc < 3) {
        printf("Not enough arguments.\n");
        return 1;
    }

    try {
        num_threads = std::stoi(argv[1]);
        matrix_size = std::stoi(argv[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << "\n";
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Argument out of range: " << e.what() << "\n";
        return 1;
    }

    // Default settings in case of wrong input.
    // Based on stoi() conversion behavior.
    if (matrix_size == 0) {
        matrix_size = 20000;
    }

    std::cout << "Number of threads used: " << num_threads << "\n";
    std::cout << "The size of the matrix: " << matrix_size << " x " << matrix_size << "\n";

    tserial = run(matrix_size, 1, "serial");
    std::cout << "Elapsed time (serial): " << std::setprecision(12) << tserial << "\n";

    if (num_threads > 1) {
        std::vector<std::string> schedules = {"static", "dynamic"};
        for (const auto& schedule : schedules) {
            tparallel = run(matrix_size, num_threads, schedule);
            std::cout << "Elapsed time (parallel, schedule = " << schedule << "): " << std::setprecision(12) << tparallel << "\n";
            std::cout << "Speedup: " << std::setprecision(6) << tserial / tparallel << "\n";  
        }
    }

    return 0;
}