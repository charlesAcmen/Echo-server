#pragma once
#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "ThreadPool.h"
#include <iostream>

struct LogMessage {
    std::string text;
};

// Sink 抽象
class Sink {
    public:
        virtual ~Sink() {}
        virtual void log(const LogMessage& msg) = 0;
};

class ConsoleSink : public Sink {
    public:
        ConsoleSink(std::ostream& = std::cout);
        void log(const LogMessage&) override;
    private:
        std::ostream& out;
};

class AsyncLogger {
    public:
        AsyncLogger(std::shared_ptr<Sink> sink = std::make_shared<ConsoleSink>(),size_t workerThreads = 1);
        static AsyncLogger& getLogger();
        void log(const std::string& text);
    private:
        
        //ensure singleton pattern
        AsyncLogger(const AsyncLogger&) = delete;
        AsyncLogger& operator=(const AsyncLogger&) = delete;


        //inline:C++17,initialize static member directly in class reference
        //cannot use static AsyncLogger& g_logger;cause 
        inline static std::unique_ptr<AsyncLogger> g_logger = nullptr;
        //flag for instantiation once
        inline static std::once_flag flag;



        std::shared_ptr<Sink> sink;
        ThreadPool pool;
};
