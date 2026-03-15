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
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    // 3. 监听 socket
    if (listen(sockfd, 1024) < 0)
    {
        printf("socket listen error: %d %s\n", errno, strerror(errno));
    }
    else
    {
        printf("socket listening ...\n");
    }

    while (true)
    {
        // 4. 接受客户端连接
        int confd = accept(sockfd, nullptr, nullptr); // 连接成功返回一个新的socket
        if (confd < 0)
        {
            printf("socket accept error: %d %s\n", errno, strerror(errno));
            return 1;
        }

        char buf[1024] = {0};

        // 5. 接收客户端数据
        size_t len = recv(confd, buf, sizeof(buf), 0);
        printf("recived data from client: %s\n", buf);

        // 6. 向客户端发送数据
        strcat(buf, " [processed]");
        send(confd, buf, strlen(buf), 0);
    }

    // 7. 关闭 socket
    close(sockfd);
    return 0;
}