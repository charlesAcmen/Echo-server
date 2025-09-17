#pragma once
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include "Logger.h"
//set fd to non-blocking mode
inline int setNonBlocking(int fd){
    //get current flags
    //fcntl:file control
    int flags = fcntl(fd, F_GETFL, 0); 
    if (flags == -1) {
        Logger::ERROR("fcntl F_GETFL failed");
        return -1;
    }
    //append O_NONBLOCK flag
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
        Logger::ERROR("fcntl F_SETFL failed");
        return -1;
    }
    Logger::INFO("Set non-blocking mode for fd: " + std::to_string(fd));
    return 0;
}


class Acceptor {
    public:
        //65535 ports,unsigned
        Acceptor(uint16_t port);
        ~Acceptor();
        int fd() const;
        int acceptFd();
    private:
        int listenFd{-1};
};