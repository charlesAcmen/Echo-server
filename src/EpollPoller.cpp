#include "EpollPoller.h"
#include "Logger.h"
#include <stdexcept>
EpollPoller::EpollPoller(){ 
    // create epoll instance
    epfd = epoll_create1(0); 
    if(epfd==-1) 
        throw std::runtime_error("epoll_create1"); 
    events.resize(1024); 
}
EpollPoller::~EpollPoller(){ 
    if(epfd!=-1) 
        close(epfd); 
}
//RAII: Resource Acquisition Is Initialization
//int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//fd: the file descriptor to be monitored
//events: the events to be monitored
//uint32_t:bit mask, can be ORed together to represent multiple events
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
//-1：infinite wait,blocking
//0: non-blocking, return immediately
//>0: wait for timeout in milliseconds
//return number of events
int EpollPoller::wait(int timeoutMs){ 
    if (timeoutMs < -1) {
        Logger::WARN("Invalid timeout value: " + std::to_string(timeoutMs));
        timeoutMs = -1;  // 无限等待
    }
    
    int numEvents = epoll_wait(epfd, events.data(), (int)events.size(), timeoutMs);
    
    if (numEvents == -1) {
        if (errno != EINTR) {  // 忽略信号中断
            Logger::ERROR("epoll_wait error");
        }
        return -1;
    } else if (numEvents == (int)events.size()) {
        //returned number of events is equal to the size of the events array, we may have more events to handle
        events.resize(events.size() * 2);
        Logger::INFO("Expanded events array to: " + std::to_string(events.size()));
    }
    
    return numEvents;
    // return epoll_wait(
    //     epfd, 
    //     events.data(), 
    //     (int)events.size(), 
    //     timeoutMs); 
}
epoll_event* EpollPoller::getEvents() { 
    return events.data(); 
}