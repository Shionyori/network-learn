#pragma once

#include <functional>
#include <memory>
#include <string>

#include "buffer.h"
#include "channel.h"
#include "eventloop.h"
#include "socket.h"

using namespace nl;

class Connection {
public:
    using ConnectionCallback = std::function<void(Connection*)>;
    using MessageCallback = std::function<void(Connection*, Buffer*)>;
    using CloseCallback = std::function<void(Connection*)>;

    Connection(EventLoop* loop, Socket&& socket);
    ~Connection();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    int fd() const { return socket.getFd(); }
    bool connected() const { return state == State::Connected; }

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback = cb; }

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& data);
    void shutdown();

    Buffer* inputBufferPtr() { return &inputBuffer; }
    Buffer* outputBufferPtr() { return &outputBuffer; }

private:
    enum class State {
        Connecting,
        Connected,
        Disconnecting,
        Disconnected,
    };

    void setState(State s) { state = s; }

    void handleRead();
    void handleWrite();
    void handleClose();

    void sendInLoop(const char* data, size_t len);

private:
    EventLoop* loop;
    Socket socket;
    std::unique_ptr<Channel> channel;

    State state;
    Buffer inputBuffer;
    Buffer outputBuffer;

    ConnectionCallback connectionCallback;
    MessageCallback messageCallback;
    CloseCallback closeCallback;
};
