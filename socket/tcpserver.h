#pragma once

#include "socket.h"
#include <unordered_map>
#include "channel.h"
#include <memory>
#include "eventloop.h"
#include "buffer.h"

using namespace nl;

class TcpServer {
private:
    Socket server;
    EventLoop loop;

    std::unique_ptr<Channel> serverChannel;
    
    std::unordered_map<int, Socket> clients;
    std::unordered_map<int, std::unique_ptr<Channel>> clientChannels;
    std::unordered_map<int, Buffer> clientBuffers;

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