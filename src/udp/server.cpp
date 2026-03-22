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

    // 2. 绑定 socket
    std::string ip = "127.0.0.1";
    int port = 8080;
    
    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str()); // ipv4十进制字符串->网络字节序二进制地址
    sockaddr.sin_port = htons(port); // 端口号转大端序 host -> net (short)
    
    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        printf("socket bind error: %d %s\n", errno, strerror(errno));
        return 1;
    }

    while (true)
    {
        // 3. 接收客户端数据
        char buf[1024] = {0};
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        ssize_t recv_len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (recv_len < 0)
        {
            printf("recvfrom error: %d %s\n", errno, strerror(errno));
            return 1;
        }
        else
        {
            printf("recv data from client: %s\n", buf);
        }

        // 4. 向客户端发送数据
        std::string data = "test";
        ssize_t send_len = sendto(sockfd, data.c_str(), data.size(), 0, (struct sockaddr*)&client_addr, client_addr_len);
        if (send_len < 0)
        {
            printf("sendto error: %d %s\n", errno, strerror(errno));
            return 1;
        }
    }

    // 5. 关闭 socket
    close(sockfd);
    return 0;
}