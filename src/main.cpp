#include "Logger.h"
#include <iostream>
#include "Server.h"

int main(int argc, char** argv){
    uint16_t port = 9000;
    if(argc>1) port = (uint16_t)atoi(argv[1]);
    Logger::INFO("Starting echo server on port " + std::to_string(port));
    // Create server with 4 worker threads
    TcpServer server(port, 4);


    // register callbacks
    server.onConnection = 
        [](const TcpConnection::Ptr& conn){ 
            Logger::INFO(
                "New connection fd=" + std::to_string(conn->getFd())
            ); 
        };
    server.onMessage = 
        [&server](const TcpConnection::Ptr& conn, const std::string& msg){
            // Echo back (append to output buffer and let main loop flush)
            conn->outputBuffer().append(msg.data(), msg.size());
        };
    server.start();
    return 0;
}