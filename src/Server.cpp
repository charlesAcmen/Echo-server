#include "Server.h"
TcpServer::TcpServer(uint16_t port, size_t workerThreads)
        : acceptor(port), poller(), pool(workerThreads){
    poller.addFd(acceptor.fd(), EPOLLIN | EPOLLET);
}
void TcpServer::start(){
    Logger::INFO("Server start loop");
    while(true){
        int n = poller.wait(1000);
        if(n<0) { 
            Logger::ERROR("epoll wait error"); 
            break; 
        }
        auto *events = poller.getEvents();
        for(int i=0;i<n;i++){
            int fd = events[i].data.fd; 
            uint32_t ev = events[i].events;
            if(fd == acceptor.fd()) { 
                handleAccept(); 
            }
            else {
                auto it = conns.find(fd);
                if(it==conns.end()) { 
                    poller.delFd(fd); 
                    close(fd); 
                    continue; 
                }
                auto conn = it->second;
                if(ev & (EPOLLHUP | EPOLLERR)) { 
                    handleClose(conn); 
                }
                else if(ev & EPOLLIN) { 
                    handleRead(conn); 
                }
                else if(ev & EPOLLOUT) { 
                    handleWrite(conn); 
                }
            }
        }
    }
}

void TcpServer::handleAccept(){
    while(true){
        int c = acceptor.acceptFd();
        if(c==-1) 
            break;
        auto conn = std::make_shared<TcpConnection>(c);
        conns[c] = conn;
        poller.addFd(c, EPOLLIN | EPOLLET | EPOLLOUT);
        if(onConnection) 
            onConnection(conn);
    }
}

void TcpServer::handleRead(const TcpConnection::Ptr& conn){
    pool.enqueue([this, conn]{
            char buf[4096];
            while(true){
                ssize_t n = ::recv(conn->getFd(), buf, sizeof(buf), 0);
                if(n>0){
                // append and call message callback
                conn->inputBuffer().append(buf, (size_t)n);
                std::string msg = conn->inputBuffer().retrieveAllAsString();
                if(onMessage) 
                    onMessage(conn, msg);
                }else if(n==0){ // closed by peer
                    this->handleClose(conn);
                    break;
                } else { 
                    if(errno==EAGAIN || errno==EWOULDBLOCK) 
                        break; 
                    else { 
                        this->handleClose(conn);
                        break; 
                    } 
                }
            }
        }
    );
}

void TcpServer::handleWrite(const TcpConnection::Ptr& conn){
    // try to flush output buffer
    auto &out = conn->outputBuffer();
    const char* data = out.peek(); // may be nullptr
    size_t remain = out.readableBytes();
    while(remain>0){
        ssize_t n = ::send(conn->getFd(), data, remain, 0);
        if(n>0){ 
            out.retrieve((size_t)n); 
            data = out.peek(); 
            remain = out.readableBytes(); 
        }
        else if(n==-1){ 
            if(errno==EAGAIN || errno==EWOULDBLOCK) 
                break; 
            else {
                handleClose(conn); 
                break; 
            } 
        }
    }
}

void TcpServer::handleClose(const TcpConnection::Ptr& conn){
    int fd = conn->getFd();
    poller.delFd(fd);
    conns.erase(fd);
}
