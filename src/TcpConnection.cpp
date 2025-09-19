#include "TcpConnection.h"
#include "Logger.h"
#include <unistd.h>
TcpConnection::TcpConnection(int fd): fd(fd){
    this->onClose =
        [this](const Ptr& conn){
            Logger::INFO("connection closed fd=" + std::to_string(conn->fd));
        };
}
TcpConnection::~TcpConnection(){ 
    if(fd!=-1) 
        close(fd); 
}
int TcpConnection::getFd() const { return fd; }
Buffer& TcpConnection::inputBuffer(){ return inBuf; }
Buffer& TcpConnection::outputBuffer(){ return outBuf; }