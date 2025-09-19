#include "Acceptor.h"  
#include "logger/Logger.h"
Acceptor::Acceptor(uint16_t port){
    //create tcp socket,AF_INET:ipv4,SOCK_STREAM:tcp
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd==-1){
        AsyncLogger::getLogger().log("tcp socket creation failed");
        throw std::runtime_error("socket: CREATION");
    }
    int on = 1;
    //set SO_REUSEADDR option
    //fast reuse address and port in TIME_WAIT state
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        close(fd);
        AsyncLogger::getLogger().log("setsockopt SO_REUSEADDR failed");
        throw std::runtime_error("setsockopt: (SO_REUSEADDR)");
    }
    struct sockaddr_in addr{}; 
    //address family:ipv4 in line with socket creation
    addr.sin_family = AF_INET; 
    //0.0.0.0,that port of any network interfaces
    addr.sin_addr.s_addr = INADDR_ANY; 
    //convert port which is in host byte order to network byte order(big-endian)
    //ntohs
    addr.sin_port = htons(port);
    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr))!=0) 
        //EADDRINUSE,previlege
        throw std::runtime_error("bind");
    //maintain a queue of connection not yet accepted for kernel
    //SOMAXCONN:maximum length of the queue
    if(listen(fd, SOMAXCONN)!=0) 
        //fd invalid
        throw std::runtime_error("listen");
    //listening socket set non-blocking
    //no blocking on accept,recv,send
    setNonBlocking(fd);      
    listenFd = fd;
    AsyncLogger::getLogger().log("server listening on port " + std::to_string(port));  
}
Acceptor::~Acceptor(){ 
    if(listenFd!=-1)
        close(listenFd); 
}
int Acceptor::fd() const { return listenFd; }
int Acceptor::acceptFd(){
    //client address info
    struct sockaddr_in peer{}; 
    socklen_t len = sizeof(peer);
    //global namespace
    int conn = ::accept(listenFd, (struct sockaddr*)&peer, &len);
    if(conn==-1){
        //no ready connections to accept
        return -1;
    }
    setNonBlocking(conn);
    return conn;
}