#include "Buffer.h"
#include <stdexcept>
void Buffer::append(const char* data, size_t len){
    //till the end
    buf.insert(buf.end(), data, data+len);
}
size_t Buffer::readableBytes() const{ 
    return buf.size() - readIndex; 
}
//peek at data not moving readIndex
const char* Buffer::peek() const{ 
    if (readIndex >= buf.size()) {
        throw std::out_of_range("Read index out of bounds");
    }
    return buf.data() + readIndex; 
}
//move readIndex forward
void Buffer::retrieve(size_t len) {
    if (len > readableBytes()) {
        throw std::out_of_range("Trying to retrieve more than readable bytes");
    }
    
    readIndex += len;
    
    if (readIndex >= buf.size()) {
        // 所有数据都已读取，重置缓冲区
        clear();
    }
}
std::string Buffer::retrieveAllAsString(){ 
    std::string s(peek(), peek() + readableBytes());
    clear();
    return s;
} 


std::string Buffer::retrieveAsString(size_t len){ 
    if (len > readableBytes()) {
        throw std::out_of_range("Not enough readable bytes");
    }
    std::string s(peek(), peek() + len);
    retrieve(len);
    return s;
}



//clear buffer
void Buffer::clear(){
    buf.clear();
    readIndex = 0;
}