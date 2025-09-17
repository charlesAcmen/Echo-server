#pragma once
#include "Buffer.h"
#include <functional>
#include <memory>
#include <unistd.h>

class TcpConnection {
    public:
        using Ptr = std::shared_ptr<TcpConnection>;
        TcpConnection(int fd);
        ~TcpConnection();
        int getFd() const;
        Buffer& inputBuffer();
        Buffer& outputBuffer();
        // Callbacks
        std::function<void(const Ptr&)> onConnection; // new connection established
        std::function<void(const Ptr&, const std::string&)> onMessage; // message arrived
        std::function<void(const Ptr&)> onClose; // closed
    private:
        int fd{-1};
        Buffer inBuf; 
        Buffer outBuf;
};