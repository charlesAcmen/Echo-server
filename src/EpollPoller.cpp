#include "EpollPoller.h"
EpollPoller::EpollPoller(){ epfd = epoll_create1(0); if(epfd==-1) throw std::runtime_error("epoll_create1"); events.resize(1024); }
EpollPoller::~EpollPoller(){ if(epfd!=-1) close(epfd); }
void EpollPoller::addFd(int fd, uint32_t events){
    struct epoll_event ev{}; 
    ev.events = events; 
    ev.data.fd = fd; 
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)==-1) 
        throw std::runtime_error("epoll_ctl add"); 
}
void EpollPoller::modFd(int fd, uint32_t events){ 
    struct epoll_event ev{}; 
    ev.events = events; 
    ev.data.fd = fd; 
    if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev)==-1) 
        throw std::runtime_error("epoll_ctl mod"); 
}
void EpollPoller::delFd(int fd){
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr); 
}
int EpollPoller::wait(int timeoutMs){ 
    return epoll_wait(
        epfd, 
        events.data(), 
        (int)events.size(), 
        timeoutMs); 
}
epoll_event* EpollPoller::getEvents() { 
    return events.data(); 
}