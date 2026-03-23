#include "udpclient.h"
#include <iostream>

UdpClient::~UdpClient() {
    client.close();
}

void UdpClient::init()
{
    if (!client.create(AF_INET, SOCK_DGRAM, IPPROTO_UDP))
    {
        std::cerr << "Client create failed\n";
        return;
    }
}

ssize_t UdpClient::sendTo(const std::string& data, const std::string &ip, int port)
{
    return client.sendTo(data, ip, port);
}

std::string UdpClient::recvFrom(size_t max_len, std::string &ip, int &port)
{
    return client.recvFrom(max_len, ip, port);
}