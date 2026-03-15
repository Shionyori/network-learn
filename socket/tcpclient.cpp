#include "tcpclient.h"

TcpClient::~TcpClient()
{
    client.close();
}

bool TcpClient::connect(const std::string& ip, int port)
{
    if (!client.create(AF_INET, SOCK_STREAM, IPPROTO_TCP))
    {
        return false;
    }
    if (!client.connect(ip, port))
    {
        return false;
    }
    return true;
}

ssize_t TcpClient::send(const std::string& data)
{
    return client.send(data);
}

std::string TcpClient::recv(size_t max_len)
{
    return client.recv(max_len);
}