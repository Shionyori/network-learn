#include "tcpserver.h"
#include <iostream>
#include <cerrno>
#include "util.h"

TcpServer::TcpServer() : loop(1024), serverChannel(nullptr) {}

TcpServer::~TcpServer() = default; // Connection 的析构会自动清理资源，RAII 管理连接对象

bool TcpServer::start(const std::string& ip, int port)
{
    if (!server.create(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    {
        std::cerr << "Server create failed\n";
        return false;
    }

    set_non_blocking(server.getFd()); // 设置服务器套接字为非阻塞模式

    if (!server.bind(ip, port))
    {
        std::cerr << "Server bind failed\n";
        return false;
    }
    if (!server.listen(128))
    {
        std::cerr << "Server listen failed\n";
        return false;
    }

    serverChannel = std::make_unique<Channel>(server.getFd(), &server);
    serverChannel->setEvents(EPOLLIN); // 监听可读事件（即有新连接到来）
    serverChannel->setReadCallback([this]() { this->handleAccept(); }); // 绑定回调
    
    // epoll 添加服务器套接字
    loop.addChannel(serverChannel.get());

    std::cout << "Server listening on " << ip << ":" << port << std::endl;
    return true;
}

void TcpServer::run()
{
    loop.loop();
}

void TcpServer::handleAccept()
{
    while (true)
    {
        Socket client = server.accept();
        if (!client.isValid())
        {
            break;
        }

        set_non_blocking(client.getFd()); // 设置客户端套接字为非阻塞模式

        int clientFd = client.getFd();
        std::cout << "New client connected: " << clientFd << std::endl;

        auto connection = std::make_shared<Connection>(&loop, std::move(client));

        // 设置回调
        connection->setConnectionCallback([this](Connection* conn) { onConnection(conn); });
        connection->setMessageCallback([this](Connection* conn, Buffer* buffer) { onMessage(conn, buffer); });
        connection->setCloseCallback([this](Connection* conn) { onClose(conn); });

        // 先存储，再建立连接，避免回调期间查不到该连接。
        Connection* connPtr = connection.get();
        connections[clientFd] = connection;

        // 建立连接（注册事件，调用用户回调）
        connPtr->connectEstablished();
        
    }
}

void TcpServer::onConnection(Connection* conn)
{
    if (connectionCallback)
    {
        connectionCallback(conn);
    }
}

void TcpServer::onMessage(Connection* conn, Buffer* buffer)
{
    if (messageCallback)
    {
        messageCallback(conn, buffer);
    }
}

void TcpServer::onClose(Connection* conn)
{
    int fd = conn->fd();

    if (closeCallback)
    {
        closeCallback(conn);
    }

    // 不能在 Connection 自己的关闭回调栈里直接销毁自身对象
    loop.queueInLoop([this, fd]() {
        connections.erase(fd);
    });
}