#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>


class ThreadPool {
    public:
        ThreadPool(size_t n);
        ~ThreadPool();
        void enqueue(std::function<void()> f);
    private:
        void workerLoop();

        std::vector<std::thread> workers; 
        std::queue<std::function<void()>> tasks; 
        std::mutex m; 
        std::condition_variable cv; 
        std::atomic<bool> stop{false};
};