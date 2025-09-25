#pragma once
#include <fcntl.h>
#include "logger/Logger.h"
//set fd to non-blocking mode
inline int setNonBlocking(int fd){
    //get current flags
    //fcntl:file control
    int flags = fcntl(fd, F_GETFL, 0); 
    if (flags == -1) {
        AsyncLogger::getLogger().log("fcntl F_GETFL failed");
        return -1;
    }
    //append O_NONBLOCK flag
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK | FD_CLOEXEC) == -1) {
        AsyncLogger::getLogger().log("fcntl F_SETFL failed");
        return -1;
    }
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