#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>


inline int setNonBlocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0); 
    fcntl(fd, F_SETFL, flags | O_NONBLOCK); 
    return 0;
}


class Acceptor {
    public:
        Acceptor(uint16_t port);
        ~Acceptor();
        int fd() const;
        int acceptFd();
    private:
        int listenFd{-1};
};