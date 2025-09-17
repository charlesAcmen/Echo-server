#include "Buffer.h"
#include <stdexcept>
#include <iostream>
#include <cstring>
void Buffer::append(const char* data, size_t len){
    //till the end
    if (writeIndex + len > buf.size()) {
        buf.resize(writeIndex + len);
    }
    std::memcpy(buf.data() + writeIndex, data, len);
    writeIndex += len;
}
size_t Buffer::readableBytes() const{ 
    return writeIndex - readIndex; 
}
//peek at data not moving readIndex
const char* Buffer::peek() const{ 
    if (readIndex >= writeIndex) {
        throw std::out_of_range("No data to read");
    }
    return buf.data() + readIndex; 
}
//move readIndex forward
void Buffer::retrieve(size_t len) {
    if (len > readableBytes()) {
        throw std::out_of_range("Trying to retrieve more than readable bytes");
    }
    
    readIndex += len;
    
    if (readIndex == writeIndex) {
        readIndex = 0;
        writeIndex = 0;
    }
}
std::string Buffer::retrieveAllAsString(){ 
    if (readableBytes() == 0) return "";
    std::string s(peek(), readableBytes());
    retrieve(readableBytes());
    return s;
} 


std::string Buffer::retrieveAsString(size_t len){ 
    if (len > readableBytes()) {
        throw std::out_of_range("Not enough readable bytes");
    }
    std::string s(peek(),len);
    retrieve(len);
    return s;
}



//clear buffer
void Buffer::clear(){
    buf.clear();
    readIndex = 0;
    writeIndex = 0;
}