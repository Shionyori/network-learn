#pragma once

#include "socket.h"
#include "epoll.h"
#include <unordered_map>
#include "channel.h"
#include <memory>

using namespace nl;

class TcpServer {
private:
    Socket server;
    Epoll ep;

    std::unique_ptr<Channel> serverChannel;
    
    std::unordered_map<int, Socket> clients;
    std::unordered_map<int, std::unique_ptr<Channel>> clientChannels;

public:
    TcpServer();
    ~TcpServer();

    bool start(const std::string& ip, int port);
    void run();

private:
    void handleAccept();
    void handleRead(int clientFd);
    void closeClient(int clientFd);
};