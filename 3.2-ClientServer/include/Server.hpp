#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <condition_variable>
#include <functional>
#include <optional>
#include <future>
#include <atomic>

template <typename Task, typename Result>
class Server {
public:
    Server() : running_(false), next_id_(1) {}

    ~Server() {
        if (running_) {
            stop();
        }
    }

    void start() {
        if (!running_) {
            running_ = true;
            server_thread_ = std::thread(&Server::process_tasks, this);
        }
    }

    void stop() {
        if (running_) {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                running_ = false;
                cv_.notify_all();
            }
            server_thread_.join();
        }
    }

    size_t add_task(Task task) {
        std::unique_lock<std::mutex> lock(mutex_);
        size_t id = next_id_++;
        
        auto promise = std::make_shared<std::promise<Result>>();
        futures_[id] = promise->get_future();
        
        task_queue_.push({id, [task, promise]() {
            try {
                Result result = task();
                promise->set_value(result);
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        }});
        
        cv_.notify_one();
        return id;
    }

    std::optional<Result> request_result(size_t id) {
        std::unique_lock<std::mutex> lock(mutex_);
        auto it = futures_.find(id);
        if (it == futures_.end()) {
            return std::nullopt;
        }
        
        if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            return it->second.get();
        }
        return std::nullopt;
    }

    Result get_result_blocking(size_t id) {
        std::future<Result> future;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto it = futures_.find(id);
            if (it == futures_.end()) {
                throw std::runtime_error("Invalid task ID");
            }
            future = std::move(it->second);
            futures_.erase(it);
        }
        return future.get();
    }

private:
    void process_tasks() {
        while (running_) {
            std::pair<size_t, std::function<void()>> task_pair;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return !task_queue_.empty() || !running_; });

                if (!running_ && task_queue_.empty()) {
                    break;
                }

                if (!task_queue_.empty()) {
                    task_pair = task_queue_.front();
                    task_queue_.pop();
                } else {
                    continue;
                }
            }

            task_pair.second();
        }
    }

private:
    std::thread server_thread_;
    std::atomic<bool> running_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::pair<size_t, std::function<void()>>> task_queue_;
    std::unordered_map<size_t, std::future<Result>> futures_;
    size_t next_id_;
};