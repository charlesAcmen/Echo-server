#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <stdexcept>


class EpollPoller {
    public:
    EpollPoller();
    ~EpollPoller();
    void addFd(int fd, uint32_t events);
    void modFd(int fd, uint32_t events);
    void delFd(int fd);
    int wait(int timeoutMs);
    epoll_event* getEvents();
    private:
    int epfd{-1}; 
    std::vector<epoll_event> events;
};