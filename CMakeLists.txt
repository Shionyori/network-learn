cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(network-learn)

add_executable(nl-server src/server.cpp)
target_link_libraries(nl-server PRIVATE mysocket)

add_executable(nl-client src/client.cpp)
target_link_libraries(nl-client PRIVATE mysocket)