#include "tcpserver.h"
#include <iostream>
#include "util.h"

TcpServer::TcpServer() : ep(1024), serverChannel(nullptr) {}

TcpServer::~TcpServer()
{
    server.close();
}

bool TcpServer::start(const std::string& ip, int port)
{
    if (!server.create(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    {
        std::cerr << "Server create failed\n";
        return false;
    }

    set_non_blocking(server.getFd()); // 设置服务端套接字为非阻塞模式

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

    // 创建服务端 Channel，并设置事件和回调函数
    serverChannel = std::make_unique<Channel>(server.getFd(), &server);
    serverChannel->setEvents(EPOLLIN); // 监听可读事件
    serverChannel->setReadCallback([this]() { handleAccept(); }); // 设置可读事件回调函数

    // epoll 监听服务端套接字的可读事件
    ep.add(serverChannel.get());

    std::cout << "Server listening on " << ip << ":" << port << std::endl;
    return true;
}

void TcpServer::run()
{
    while(true)
    {
        int n = ep.wait();

        for (int i = 0; i < n; ++i)
        {
            Channel* channel = ep.getChannel(i); // 获取就绪事件的 Channel
            channel->setRevents(ep.getEvent(i).events); // 设置就绪事件的类型
            channel->handleEvent(); // 处理事件（调用相应的回调函数）
        }
    }
}

void TcpServer::handleAccept()
{
    Socket client = server.accept();
    if (!client.isValid())
    {
        return;
    }

    int clientFd = client.getFd();
    std::cout << "New client connected: " << clientFd << std::endl;

    set_non_blocking(clientFd); // 设置客户端套接字为非阻塞模式

    // 创建客户端 Channel，并设置事件和回调函数
    auto clientChannel = std::make_unique<Channel>(clientFd);
    clientChannel->setEvents(EPOLLIN); // 监听可读事件
    clientChannel->setReadCallback([this, clientFd]() { handleRead(clientFd); }); // 设置可读事件回调函数

    // epoll 监听客户端套接字的可读事件
    ep.add(clientChannel.get());

    // 将客户端 Socket 和 Channel 存储到容器中
    clients[clientFd] = std::move(client);
    clientChannels[clientFd] = std::move(clientChannel);
}

void TcpServer::handleRead(int clientFd)
{
    char buffer[1024];

    ssize_t bytesRead = clients[clientFd].recv(buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        std::string data(buffer, bytesRead);
        std::cout << "Received from client " << clientFd << ": " << data << std::endl;

        clients[clientFd].send("Echo: " + data); // 回显数据给客户端
    }
    else
    {
        std::cout << "Client " << clientFd << " disconnected\n";
        closeClient(clientFd);
    }
}

void TcpServer::closeClient(int clientFd)
{
    ep.del(clientChannels[clientFd].get()); // 从 epoll 中删除客户端 Channel
    clientChannels.erase(clientFd); // 从容器中删除客户端 Channel
    clients.erase(clientFd); // 从容器中删除客户端 Socket
}