#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>

#include <cstring>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // 1. 创建 socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        printf("create socket error: %d %s\n", errno, strerror(errno));
        return 1;
    }
    else
    {
        printf("create socket successfully\n");
    }

    // 2. 向服务端发送数据
    std::string ip = "127.0.0.1";
    int port = 8080;

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);

    std::string data = "test";
    ssize_t send_len = sendto(sockfd, data.c_str(), data.size(), 0, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    if (send_len < 0)
    {
        printf("sendto error: %d %s\n", errno, strerror(errno));
        return 1;
    }
    else
    {
        printf("send data to server successfully\n");
    }

    // 3. 接收服务端数据
    char buf[1024] = {0};
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    ssize_t recv_len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&server_addr, &server_addr_len);
    if (recv_len < 0)
    {
        printf("recvfrom error: %d %s\n", errno, strerror(errno));
        return 1;
    }
    else
    {
        printf("recv data from server: %s\n", buf);
    }

    // 4. 关闭 socket
    close(sockfd);
    return 0;
}