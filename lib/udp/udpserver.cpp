#include "udpserver.h"
#include <iostream>

UdpServer::~UdpServer() {
    server.close();
}

void UdpServer::start(const std::string &ip, int port)
{
    if (!server.create(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
    {
        std::cerr << "Server create failed\n";
        return;
    }

    if (!server.bind(ip, port))
    {
        std::cerr << "Server bind failed\n";
        return;
    }

    std::cout << "Server listening on " << ip << ":" << port << std::endl;
}

void UdpServer::run()
{
    while (true)
    {
        std::string client_ip;
        int client_port;
        
        std::string data = server.recvFrom(1024, client_ip, client_port);
        if (!data.empty())
        {
            std::cout << "Received from " << client_ip << ":" << client_port << " - " << data << std::endl;

            // 回显数据
            server.sendTo(data, client_ip, client_port);
        }
    }
}