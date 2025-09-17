#pragma once
#include <vector>
#include <string>
/*
loop buffer
data range: [readIndex, buf.size())
*/
class Buffer {
    public:
        void append(const char* data, size_t len);
        size_t readableBytes() const;
        void retrieve(size_t len);
        std::string retrieveAllAsString();
        const char* peek() const;
        std::string retrieveAsString(size_t len);
    private:
        void clear();
        std::vector<char> buf;
        size_t readIndex = 0;
};