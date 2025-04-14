#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <functional>

#include "Server.hpp"

class Client {
public:
    Client(const std::string& name, const std::string& filename);
    
    void run_sin_tasks(Server<std::function<double()>, double>& server, int N);
    void run_sqrt_tasks(Server<std::function<double()>, double>& server, int N);
    void run_pow_tasks(Server<std::function<double()>, double>& server, int N);

private:
    std::string client_name_;
    std::string filename_;
};

#endif