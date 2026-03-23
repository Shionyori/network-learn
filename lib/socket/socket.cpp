#include "socket.h"

namespace nl {

Socket::Socket() : sockfd(-1) {}

Socket::Socket(int fd) : sockfd(fd) {}

Socket::~Socket() { close(); }

Socket::Socket(Socket&& other) : sockfd(other.sockfd) {
    other.sockfd = -1;
}

Socket& Socket::operator=(Socket&& other) {
    if (this != &other) {
        close();
        sockfd = other.sockfd;
        other.sockfd = -1;
    }
    return *this;
}

bool Socket::create(int domain, int type, int protocol) 
{
    sockfd = socket(domain, type, protocol);
    return sockfd >= 0;
}

bool Socket::bind(const std::string& ip, int port)
{
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    return ::bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

bool Socket::listen(int backlog)
{
    return ::listen(sockfd, backlog) == 0;
}

Socket Socket::accept()
{
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    return Socket(client_fd);
}

bool Socket::connect(const std::string& ip, int port)
{
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    return ::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

ssize_t Socket::send(const void* data, size_t len, int flags)
{
    return ::send(sockfd, data, len, flags);
}

ssize_t Socket::send(const std::string& data, int flags)
{
    return send(data.c_str(), data.size(), flags);
}

ssize_t Socket::recv(void* buf, size_t len, int flags)
{
    return ::recv(sockfd, buf, len, flags);
}

std::string Socket::recv(size_t max_len, int flags)
{
    std::string buf(max_len, '\0');
    ssize_t n = recv(&buf[0], max_len, flags);
    if (n > 0) {
        buf.resize(n);
    } else {
        buf.clear();
    }
    return buf;
}

ssize_t Socket::sendTo(const void* data, size_t len, const std::string &ip, int port, int flags)
{
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    return ::sendto(sockfd, data, len, flags, (struct sockaddr*)&addr, sizeof(addr));
}

ssize_t Socket::sendTo(const std::string& data, const std::string &ip, int port, int flags)
{
    return sendTo(data.c_str(), data.size(), ip, port, flags);
}

ssize_t Socket::recvFrom(void* buf, size_t len, std::string &ip, int &port, int flags)
{
    struct sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    ssize_t n = ::recvfrom(sockfd, buf, len, flags, (struct sockaddr*)&addr, &addr_len);
    if (n >= 0) {
        ip = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);
    }
    return n;
}

std::string Socket::recvFrom(size_t max_len, std::string &ip, int &port, int flags)
{
    std::string buf(max_len, '\0');
    ssize_t n = recvFrom(&buf[0], max_len, ip, port, flags);
    if (n > 0) {
        buf.resize(n);
    } else {
        buf.clear();
    }
    return buf;
}

} // namespace nl