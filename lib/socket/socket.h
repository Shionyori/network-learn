#pragma once

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace nl {

class Socket {
private:
    int sockfd;

public:
    Socket();
    Socket(int fd);
    
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket && other);
    Socket& operator=(Socket&& other);

    bool create(int domain, int type, int protocol);

    bool bind(const std::string &ip, int port);
    bool listen(int backlog);
    Socket accept();

    bool connect(const std::string &ip, int port);

    ssize_t send(const void* data, size_t len, int flags = 0);
    ssize_t send(const std::string& data, int flags = 0);

    ssize_t recv(void* buf, size_t len, int flags = 0);
    std::string recv(size_t max_len = 1024, int flags = 0);

    ssize_t sendTo(const void* data, size_t len, const std::string &ip, int port, int flags = 0);
    ssize_t sendTo(const std::string& data, const std::string &ip, int port, int flags = 0);

    ssize_t recvFrom(void* buf, size_t len, std::string &ip, int &port, int flags = 0);
    std::string recvFrom(size_t max_len, std::string &ip, int &port, int flags = 0);

    // 工具函数
    void close() {
        if (sockfd >= 0) {
            ::close(sockfd);
            sockfd = -1;
        }
    }

    bool isValid() const { return sockfd >= 0; }

    int getFd() const { return sockfd; }
};

}