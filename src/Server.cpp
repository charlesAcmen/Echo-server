#include "Server.h"
#include "Logger.h"
#include <iostream>
TcpServer::TcpServer(uint16_t port, size_t workerThreads)
        : acceptor(port), poller(), pool(workerThreads){
    //listenFd which is listening on socket
    //EPOLLIN: readable event,connection request
    //EPOLLET: edge trigger,ET.
    poller.addFd(acceptor.fd(), EPOLLIN | EPOLLET);
}
//single thread event loop handles all connections
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


            //IO多路复用

            //listening socket and connecting socket
            if(fd == acceptor.fd()) { 
                handleAccept(); 
            }
            else {
                // find connection by fd
                auto it = conns.find(fd);
                if(it==conns.end()) { 
                    //clear resource
                    poller.delFd(fd); 
                    close(fd); 
                    continue; 
                }
                //connection instance:TcpConnection::Ptr
                auto conn = it->second;
                //bit mask:ev is uint32_t,& with event type to check
                if(ev & (EPOLLHUP | EPOLLERR)) { 
                    handleClose(conn); 
                }
                else if(ev & EPOLLIN) { 
                    handleRead(conn); 
                }
                else if(ev & EPOLLOUT) { 
                    handleWrite(conn); 
                }else{
                    Logger::WARN("Unknown event: " + std::to_string(ev));
                }
            }
        }
    }
}

void TcpServer::handleAccept(){
    while(true){
        int c = acceptor.acceptFd();
        if(c==-1) 
            //no more connections
            break;
        //create TcpConnection instance by connection fd
        auto conn = std::make_shared<TcpConnection>(c);
        conns[c] = conn;
        //under surveillance by epoll,ET
        //EPOLLOUT: writable event
        //EPOLLIN: readable event
        poller.addFd(c, EPOLLIN | EPOLLET | EPOLLOUT);
        if(onConnection){
            //handle new connection
            onConnection(conn);
        }
    }
}
//event distributing
//client to server
void TcpServer::handleRead(const TcpConnection::Ptr& conn){
    pool.enqueue([this, conn]{

            char buf[4096];
            while(true){
                //receive
                ssize_t n = ::recv(conn->getFd(), buf, sizeof(buf), 0); 
                if(n>0){
                    //tcp is in stream,so first append 
                    conn->inputBuffer().append(buf, (size_t)n);
                    std::string msg = conn->inputBuffer().retrieveAllAsString();
                    // and call message callback
                    if(onMessage) 
                        onMessage(conn, msg);
                }else if(n==0){ // closed by peer
                    this->handleClose(conn);
                    break;
                } else { 
                    //read all buffer
                    if(errno==EAGAIN || errno==EWOULDBLOCK){
                        break; 
                    }
                    else { 
                        this->handleClose(conn);
                        break; 
                    } 
                }
            }


        }
    );
}
//server to client
//it is io thread that is responsible to call handleWrite,ensuring sequence and safety
void TcpServer::handleWrite(const TcpConnection::Ptr& conn){
    // try to flush output buffer
    auto &out = conn->outputBuffer();
    size_t remain = out.readableBytes();
    if (remain == 0) {
        // unsubscribe to EPOLLOUT
        poller.modFd(conn->getFd(), EPOLLIN | EPOLLET);
        return;
    } 
    const char* data;
    while(remain>0){
        //no need to use threadpool to write to socket
        data = out.peek();
        remain = out.readableBytes();
        ssize_t n = ::send(conn->getFd(), data, remain, 0);
        if(n>0){ 
            //move data that has been written out of the buffer
            out.retrieve((size_t)n); 
            remain = out.readableBytes();
        }
        else if(n==-1){ 
            //output buffer is full
            if(errno==EAGAIN || errno==EWOULDBLOCK) 
                break; 
            else {
                handleClose(conn); 
                break; 
            } 
        }
    }
    //write complete
    if (out.readableBytes() == 0) {
        poller.modFd(conn->getFd(), EPOLLIN | EPOLLET); // 不关注 EPOLLOUT
}
}

void TcpServer::handleClose(const TcpConnection::Ptr& conn){
    int fd = conn->getFd();
    poller.delFd(fd);
    conns.erase(fd);
    if(conn->onClose) conn->onClose(conn);
    ::close(fd);
}
void TcpServer::enableWriting(const TcpConnection::Ptr& conn){
    poller.modFd(conn->getFd(), EPOLLIN | EPOLLOUT | EPOLLET);
}
