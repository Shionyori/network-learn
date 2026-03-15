#include "tcpserver.h"
#include <iostream>

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
    std::cout << "Server listening on " << ip << ":" << port << std::endl;
    return true;
}

void TcpServer::run(std::function<void(Socket)> handler)
{
    while(true)
    {
        Socket client = server.accept();
        handler(std::move(client));
    }
}