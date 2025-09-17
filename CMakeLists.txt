cmake_minimum_required(VERSION 3.10)
project(epoll_echo_server)
set(CMAKE_CXX_STANDARD 17)
# Enable all warnings and optimizations for compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O0 -g")

set(SOURCE_FILES
    src/Buffer.cpp
    src/Logger.cpp
    src/EpollPoller.cpp
    src/ThreadPool.cpp
    src/Acceptor.cpp
    src/TcpConnection.cpp
    src/Server.cpp
    src/main.cpp
)

add_executable(epoll_echo_server ${SOURCE_FILES})

target_include_directories(epoll_echo_server PRIVATE ${CMAKE_SOURCE_DIR}/include)



