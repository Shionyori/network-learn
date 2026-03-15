#include "tcpserver.h"
#include <iostream>

int main()
{
    TcpServer server;

    server.setConnectionCallback([](Connection* conn) {
        if (conn->connected())
        {
            std::cout << "Client connected: " << conn->fd() << std::endl;
        }
    });

    server.setMessageCallback([](Connection* conn, Buffer* buffer) {
        std::string msg(buffer->peek(), buffer->readableBytes());
        std::cout << "Received message from client " << conn->fd() << ": " << msg << std::endl;

        conn->send("Echo: " + msg); // 回显消息给客户端

        buffer->retrieveAll(); // 处理完消息后清空缓冲区
    });

    server.setCloseCallback([](Connection* conn) {
        std::cout << "Client disconnected: " << conn->fd() << std::endl;
    });

    server.start("127.0.0.1", 8080);
    server.run();

    return 0;
}