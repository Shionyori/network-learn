#pragma once

#include "socket.h"
#include <unordered_map>
#include <memory>
#include "channel.h"
#include "eventloop.h"
#include "connection.h"

using namespace nl;

class TcpServer {
private:
    Socket server;
    EventLoop loop;

    std::unique_ptr<Channel> serverChannel;
    std::unordered_map<int, std::shared_ptr<Connection>> connections;

    Connection::ConnectionCallback connectionCallback;
    Connection::MessageCallback messageCallback;
    Connection::CloseCallback closeCallback;

public:
    TcpServer();
    ~TcpServer();

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