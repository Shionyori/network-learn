#pragma once

#include "socket.h"
#include "epoll.h"
#include <unordered_map>

using namespace nl;

class TcpServer {
private:
    Socket server;
    Epoll ep;

    std::unordered_map<int, Socket> clients;

public:
    TcpServer();
    ~TcpServer();

    bool start(const std::string& ip, int port);
    void run();
};