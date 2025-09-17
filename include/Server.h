#pragma once
#include "Acceptor.h"
#include "EpollPoller.h"
#include "TcpConnection.h"
#include "ThreadPool.h"
#include <unordered_map>
#include <memory>
#include <sys/socket.h>
#include <cstring>


class TcpServer {
    public:
        TcpServer(uint16_t port, size_t workerThreads = 4);
        void start();
        // user can set these
        std::function<void(const TcpConnection::Ptr&)> onConnection;
        std::function<void(const TcpConnection::Ptr&, const std::string&)> onMessage;
        void enableWriting(const TcpConnection::Ptr& conn);
    private:
        void handleAccept();
        void handleRead(const TcpConnection::Ptr& conn);
        void handleWrite(const TcpConnection::Ptr& conn);
        void handleClose(const TcpConnection::Ptr& conn);

        

        Acceptor acceptor;
        EpollPoller poller;
        ThreadPool pool;
        std::unordered_map<int, TcpConnection::Ptr> conns;
};