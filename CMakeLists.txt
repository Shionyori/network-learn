cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(network-learn)

set(NL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/core
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/socket
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tcp
)

add_library(nl-core STATIC
    lib/core/buffer.cpp
    lib/core/channel.h
    lib/core/connection.cpp
    lib/core/eventloop.cpp
    lib/core/eventloopthread.cpp
    lib/core/eventloopthreadpool.cpp
    lib/core/epoll.cpp
)
target_include_directories(nl-core PUBLIC ${NL_INCLUDE_DIRS})

add_library(nl-socket STATIC 
    lib/socket/socket.cpp
)
target_include_directories(nl-socket PUBLIC ${NL_INCLUDE_DIRS})

add_library(nl-tcp STATIC
    lib/tcp/tcpserver.cpp
    lib/tcp/tcpclient.cpp
)
target_link_libraries(nl-tcp PRIVATE nl-core nl-socket)
target_include_directories(nl-tcp PUBLIC ${NL_INCLUDE_DIRS})

add_executable(nl-tcpserver src/tcp/demo_tcpserver.cpp)
target_link_libraries(nl-tcpserver PRIVATE nl-tcp)

add_executable(nl-tcpclient src/tcp/demo_tcpclient.cpp)
target_link_libraries(nl-tcpclient PRIVATE nl-tcp)

add_executable(nl-udpserver src/udp/server.cpp)

add_executable(nl-udpclient src/udp/client.cpp)