#pragma once
#include <vector>
#include <string>
class Buffer {
    public:
        void append(const char* data, size_t len);
        size_t readableBytes() const;
        std::string retrieveAllAsString();
        const char* peek() const;
        void retrieve(size_t len);
    private:
        std::vector<char> buf;
        size_t readIndex = 0;
};