#include "Logger.h"
#include <iostream>
void Logger::INFO(const std::string& msg) {
    std::cout << "[INFO] " << msg << std::endl;
}
void Logger::WARN(const std::string& msg) {
    std::cout << "[WARN] " << msg << std::endl;
}
void Logger::ERROR(const std::string& msg) {
    std::cout << "[ERROR] " << msg << std::endl;
}