#include "ThreadPool.h"
#include "Logger.h"
ThreadPool::ThreadPool(size_t n): stop(false) {
    for (size_t i=0;i<n;i++) 
        workers.emplace_back(
            [this]{ this->workerLoop(); }
        );
}
ThreadPool::~ThreadPool(){ 
    stop = true;
    cv.notify_all(); 
    for(auto &t: workers) 
        if(t.joinable())
            t.join(); 
}
void ThreadPool::enqueue(std::function<void()> f){
    { 
        std::unique_lock<std::mutex> lk(m); 
        tasks.push(std::move(f)); 
    }
    cv.notify_one();
}

void ThreadPool::workerLoop(){
    while(!stop){
        std::function<void()> task;
        { 
            std::unique_lock<std::mutex> lk(m); 
            cv.wait(lk, [this]{ return stop || !tasks.empty(); }); 
            if(stop && tasks.empty()) return; 
            task = std::move(tasks.front()); 
            tasks.pop(); 
        }
        try{ 
            task(); 
        }catch(...){ Logger::WARN("task threw"); }
    }
}
