#pragma once

#include "socket.h"
#include <unordered_map>
#include <memory>
#include "channel.h"
#include "eventloop.h"
#include "connection.h"
#include "buffer.h"
#include "eventloopthreadpool.h"
#include <cstddef>

using namespace nl;

class TcpServer {
private:
    Socket server;
    EventLoop mainloop;
    size_t numThreads;

    std::unique_ptr<Channel> serverChannel;
    std::unordered_map<int, std::shared_ptr<Connection>> connections;

    std::unique_ptr<EventLoopThreadPool> threadPool;

    Connection::ConnectionCallback connectionCallback;
    Connection::MessageCallback messageCallback;
    Connection::CloseCallback closeCallback;

public:
    TcpServer(size_t numThreads = 0);
    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    bool start(const std::string& ip, int port);
    void run();

    void setConnectionCallback(const Connection::ConnectionCallback& cb) { connectionCallback = cb; }
    void setMessageCallback(const Connection::MessageCallback& cb) { messageCallback = cb; }
    void setCloseCallback(const Connection::CloseCallback& cb) { closeCallback = cb; }

private:
    void handleAccept(); // accept 新连接
    void onConnection(Connection* conn); // 连接建立/断开回调（转发给用户）
    void onMessage(Connection* conn, Buffer* buffer); // 消息到达回调（转发给用户）
    void onClose(Connection* conn); // 连接关闭回调（清理资源）
};