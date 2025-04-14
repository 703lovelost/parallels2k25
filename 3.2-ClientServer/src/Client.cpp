#include <fstream>
#include <random>
#include <cmath>
#include <iostream>

#include "include/Client.hpp"

Client::Client(const std::string& name, const std::string& filename) 
    : client_name_(name), filename_(filename) {}

void Client::run_sin_tasks(Server<std::function<double()>, double>& server, int N) {
    std::ofstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        return;
    }

    file << "Client: " << client_name_ << "\n";
    file << "Task type: sin(x)\n";
    file << "ID\tArgument\tResult\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-3.14, 3.14);

    for (int i = 0; i < N; ++i) {
        double arg = dis(gen);
        size_t task_id = server.add_task([arg]() { return std::sin(arg); });
        double result = server.get_result_blocking(task_id);
        file << task_id << "\t" << arg << "\t" << result << "\n";
    }
}

void Client::run_sqrt_tasks(Server<std::function<double()>, double>& server, int N) {
    std::ofstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        return;
    }

    file << "Client: " << client_name_ << "\n";
    file << "Task type: sqrt(x)\n";
    file << "ID\tArgument\tResult\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1000);

    for (int i = 0; i < N; ++i) {
        double arg = dis(gen);
        size_t task_id = server.add_task([arg]() { return std::sqrt(arg); });
        double result = server.get_result_blocking(task_id);
        file << task_id << "\t" << arg << "\t" << result << "\n";
    }
}

void Client::run_pow_tasks(Server<std::function<double()>, double>& server, int N) {
    std::ofstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        return;
    }

    file << "Client: " << client_name_ << "\n";
    file << "Task type: pow(x, y)\n";
    file << "ID\tBase\tExponent\tResult\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis_base(0, 10);
    std::uniform_real_distribution<> dis_exp(0, 5);

    for (int i = 0; i < N; ++i) {
        double base = dis_base(gen);
        double exp = dis_exp(gen);
        size_t task_id = server.add_task([base, exp]() { return std::pow(base, exp); });
        double result = server.get_result_blocking(task_id);
        file << task_id << "\t" << base << "\t" << exp << "\t" << result << "\n";
    }
}