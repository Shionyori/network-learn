#include "tcpserver.h"
#include <iostream>

TcpServer::TcpServer() : ep(1024) {}

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

    // epoll 监听服务端套接字的可读事件
    ep.add(server.getFd(), EPOLLIN);

    std::cout << "Server listening on " << ip << ":" << port << std::endl;
    return true;
}

void TcpServer::run()
{
    while(true)
    {
        int n = ep.wait(); // 等待事件发生，返回就绪事件数量

        for (int i = 0; i < n; i++)
        {
            auto ev = ep.getEvent(i); // 获取就绪事件
            
            int fd = ev.data.fd;

            if (fd == server.getFd())
            {
                Socket client = server.accept();

                if(client.isValid())
                {
                    std::cout << "New client connected: " << client.getFd() << std::endl;
                    ep.add(client.getFd(), EPOLLIN); // epoll 监听客户端套接字的可读事件

                    clients.emplace(client.getFd(), std::move(client));
                }
            }
            else
            {
                char buffer[1024];
                ssize_t bytesRead = clients[fd].recv(buffer, sizeof(buffer));

                if (bytesRead > 0)
                {
                    std::cout << "Received from client " << fd << ": " << std::string(buffer, bytesRead) << std::endl;
                    clients[fd].send(buffer, bytesRead);
                }
                else
                {
                    std::cout << "Client " << fd << " disconnected\n";
                    ep.del(fd); // 从 epoll 中移除客户端套接字
                    clients.erase(fd); // 会触发 Socket 的析构函数从而关闭套接字
                }
            } 
        }
    }
}