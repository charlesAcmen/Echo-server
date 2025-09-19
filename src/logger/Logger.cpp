#include "logger/Logger.h"
ConsoleSink::ConsoleSink(std::ostream& os): out(os) {}
void ConsoleSink::log(const LogMessage& msg){
    out << "[INFO] " << msg.text << std::endl;
}   

AsyncLogger::AsyncLogger(std::shared_ptr<Sink> sink,size_t workerThreads)
    : sink(std::move(sink)), pool(workerThreads){}
AsyncLogger& AsyncLogger::getLogger() { 
    std::call_once(flag, []() {
        g_logger = std::make_unique<AsyncLogger>();
    });
    return *g_logger;
}
void AsyncLogger::log(const std::string& text){
    LogMessage msg{text};
    pool.enqueue([this, msg]{ sink->log(msg); });
}