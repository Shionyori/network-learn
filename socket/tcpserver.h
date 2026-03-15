#pragma once

#include "socket.h"
#include <functional>

using namespace nl;

class TcpServer {
private:
    Socket server;

public:
    TcpServer() = default;
    ~TcpServer();

    bool start(const std::string& ip, int port);
    void run(std::function<void(Socket)> handler);
};