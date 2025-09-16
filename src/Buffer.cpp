#include "Buffer.h"
void Buffer::append(const char* data, size_t len){
    buf.insert(buf.end(), data, data+len);
}
size_t Buffer::readableBytes() const{ 
    return buf.size() - readIndex; 
}
std::string Buffer::retrieveAllAsString(){ 
    std::string s(buf.begin()+readIndex, buf.end()); 
    readIndex = buf.size(); 
    return s;
} 
const char* Buffer::peek() const{ 
    return buf.data() + readIndex; 
}
void Buffer::retrieve(size_t len){ 
    readIndex += len; 
    if (readIndex == buf.size()) {
        buf.clear();
        readIndex = 0; 
    }
}