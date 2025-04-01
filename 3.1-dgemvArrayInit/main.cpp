#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

void initialize(std::vector<double>& vector, int start_index, int end_index, int matrix_size, std::vector<double>& matrix) {
    for (int i = start_index; i < end_index; i++) {
        for (int j = 0; j < matrix_size; j++) {
            matrix[i * matrix_size + j] = i + j;
        }
    }
    for (int j = 0; j < matrix_size; j++) { 
        vector[j] = j;
    }
}

void multiplication(std::vector<double>& vector, std::vector<double>& matrix, std::vector<double>& result, int start_index, int end_index, int matrix_size) {
    for (int i = start_index; i < end_index; i++) {
        result[i] = 0;
        for (int j = 0; j < matrix_size; j++) {
            result[i] += matrix[i * matrix_size + j] * vector[j];
        }
    }
}

int main(int argc, char* argv[]) {
    int matrix_size, num_threads;

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

    std::vector<double> vector_parallel(matrix_size);
    std::vector<double> matrix_parallel(matrix_size * matrix_size);
    std::vector<double> result_parallel(matrix_size, 0);

    int chunk_size = matrix_size / num_threads;
    int start_index = 0;
    std::vector<std::jthread> threads;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_threads; i++) {
        int end_index = (i == num_threads - 1) ? matrix_size : start_index + chunk_size;
        threads.emplace_back(initialize, std::ref(vector_parallel), start_index, end_index, matrix_size, std::ref(matrix_parallel));
        start_index = end_index;
    }

    threads.clear();
    start_index = 0;

    for (int i = 0; i < num_threads; i++) {
        int end_index = (i == num_threads - 1) ? matrix_size : start_index + chunk_size;
        threads.emplace_back(multiplication, std::ref(vector_parallel), std::ref(matrix_parallel), std::ref(result_parallel), start_index, end_index, matrix_size);
        start_index = end_index;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start; 

    std::vector<double> vector_serial(matrix_size);
    std::vector<double> matrix_serial(matrix_size * matrix_size);
    std::vector<double> result_serial(matrix_size, 0);
    start_index = 0;

    auto serial_start = std::chrono::high_resolution_clock::now();
    initialize(vector_serial, start_index, matrix_size, matrix_size, matrix_serial);
    multiplication(vector_serial, matrix_serial, result_serial, 0, matrix_size, matrix_size);
    auto serial_end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> serial_time = serial_end - serial_start;

    std::cout << "Elapsed time (T1): " << serial_time.count() << "\n";
    std::cout << "Elapsed time (T" << num_threads << "): " << elapsed_seconds.count() << "\n";

    double speedup = serial_time.count() / elapsed_seconds.count();
    std::cout << "Speedup: " << std::setprecision(6) << speedup << '\n';

    return 0;
}