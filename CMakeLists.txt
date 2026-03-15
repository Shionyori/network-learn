cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(network-learn)

include_directories(socket)

add_library(mysocket STATIC
    socket/socket.cpp
    socket/epoll.cpp
    socket/eventloop.cpp
    socket/tcpclient.cpp
    socket/tcpserver.cpp
)

target_include_directories(mysocket PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/socket
)

add_executable(nl-server src/demo_server.cpp)
target_link_libraries(nl-server PRIVATE mysocket)

add_executable(nl-client src/demo_client.cpp)
target_link_libraries(nl-client PRIVATE mysocket)