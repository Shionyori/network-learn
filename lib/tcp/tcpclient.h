#pragma once

#include "socket.h"

using namespace nl;

class TcpClient {
private:
    Socket client;

public:
    TcpClient() = default;
    ~TcpClient();

    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    bool connect(const std::string& ip, int port);
    ssize_t send(const std::string& data);
    std::string recv(size_t max_len = 1024);
};