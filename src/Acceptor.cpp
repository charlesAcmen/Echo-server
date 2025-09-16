#include "Acceptor.h"  
Acceptor::Acceptor(uint16_t port){
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd==-1) 
        throw std::runtime_error("socket");
    int on = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    struct sockaddr_in addr{}; 
    addr.sin_family = AF_INET; 
    addr.sin_addr.s_addr = INADDR_ANY; 
    addr.sin_port = htons(port);
    if(bind(listenFd, (struct sockaddr*)&addr, sizeof(addr))!=0) 
        throw std::runtime_error("bind");
    if(listen(listenFd, SOMAXCONN)!=0) 
        throw std::runtime_error("listen");
    setNonBlocking(listenFd);        
}
Acceptor::~Acceptor(){ 
    if(listenFd!=-1)
        close(listenFd); 
}
int Acceptor::fd() const { return listenFd; }
int Acceptor::acceptFd(){
    struct sockaddr_in peer{}; 
    socklen_t len = sizeof(peer);
    int conn = ::accept(listenFd, (struct sockaddr*)&peer, &len);
    if(conn==-1) 
        return -1;
    setNonBlocking(conn);
    return conn;
}