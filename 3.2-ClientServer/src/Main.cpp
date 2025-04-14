#include <iostream>
#include <fstream>
#include <thread>
#include <sstream>
#include <cmath>
#include <cstdlib>

#include "include/Client.hpp"

void test_results() {
    {
        std::ifstream file_sin("sin_results.txt");
        if (!file_sin.is_open()) {
            std::cerr << "Failed to open sin_results.txt for testing" << std::endl;
            return;
        }

        std::ifstream file_sqrt("sqrt_results.txt");
        if (!file_sqrt.is_open()) {
            std::cerr << "Failed to open sqrt_results.txt for testing" << std::endl;
            return;
        }

        std::ifstream file_pow("pow_results.txt");
        if (!file_pow.is_open()) {
            std::cerr << "Failed to open pow_results.txt for testing" << std::endl;
            return;
        }

        std::string line;
        int tested = 0;

        // Testing sin.
        std::getline(file_sin, line);
        std::getline(file_sin, line);
        std::getline(file_sin, line);

        while (std::getline(file_sin, line)) {
            std::istringstream iss(line);
            size_t id;
            double arg, result;
            if (iss >> id >> arg >> result) {
                double expected = std::sin(arg);
                if (std::abs(expected - result) > 1e-3) {
                    std::cerr << "Test failed for sin task ID " << id 
                              << ": expected " << expected << ", got " << result << std::endl;
                }
                tested++;
            }
        }
        std::cout << "Tested " << tested << " sin tasks" << std::endl;

        // Testing sqrt.
        std::getline(file_sqrt, line);
        std::getline(file_sqrt, line);
        std::getline(file_sqrt, line);

        tested = 0;
        while (std::getline(file_sqrt, line)) {
            std::istringstream iss(line);
            size_t id;
            double arg, result;
            if (iss >> id >> arg >> result) {
                double expected = std::sqrt(arg);
                if (std::abs(expected - result) > 1e-3) {
                    std::cerr << "Test failed for sqrt task ID " << id 
                              << ": expected " << expected << ", got " << result << std::endl;
                }
                tested++;
            }
        }
        std::cout << "Tested " << tested << " sqrt tasks" << std::endl;

        // Testing pow.
        std::getline(file_pow, line);
        std::getline(file_pow, line);
        std::getline(file_pow, line);

        tested = 0;
        while (std::getline(file_pow, line)) {
            std::istringstream iss(line);
            size_t id;
            double base, exp, result;
            if (iss >> id >> base >> exp >> result) {
                double expected = std::pow(base, exp);
                if (std::abs(expected - result) > 1e-1) {
                    std::cerr << "Test failed for pow task ID " << id 
                              << ": expected " << expected << ", got " << result << std::endl;
                }
                tested++;
            }
        }
        std::cout << "Tested " << tested << " pow tasks" << std::endl;
    }
}

int main() {
    Server<std::function<double()>, double> server;
    server.start();

    const int N = 100;

    Client sin_client("SinClient", "sin_results.txt");
    Client sqrt_client("SqrtClient", "sqrt_results.txt");
    Client pow_client("PowClient", "pow_results.txt");

    std::thread sin_thread([&sin_client, &server, N]() {
        sin_client.run_sin_tasks(server, N);
    });

    std::thread sqrt_thread([&sqrt_client, &server, N]() {
        sqrt_client.run_sqrt_tasks(server, N);
    });

    std::thread pow_thread([&pow_client, &server, N]() {
        pow_client.run_pow_tasks(server, N);
    });

    sin_thread.join();
    sqrt_thread.join();
    pow_thread.join();

    server.stop();

    test_results();

    return 0;
}