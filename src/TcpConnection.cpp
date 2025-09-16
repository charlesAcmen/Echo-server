#include "TcpConnection.h"
TcpConnection::TcpConnection(int fd): fd(fd){}
TcpConnection::~TcpConnection(){ 
    if(fd!=-1) 
        close(fd); 
}
int TcpConnection::getFd() const { return fd; }
Buffer& TcpConnection::inputBuffer(){ return inBuf; }
Buffer& TcpConnection::outputBuffer(){ return outBuf; }