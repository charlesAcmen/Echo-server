#pragma once
#include <iostream>
#include <string>
class Logger {
    public:
        static void INFO(const std::string& msg);
        static void WARN(const std::string& msg);
        static void ERROR(const std::string& msg);
};