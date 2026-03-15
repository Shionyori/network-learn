#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <cstring>

#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // 1. 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        printf("create socket error: %d %s\n", errno, strerror(errno));
        return 1;
    }
    else
    {
        printf("create socket successfully\n");
    }

    // 2. 连接服务端
    std::string ip = "127.0.0.1";
    int port = 8080;

    struct sockaddr_in sockaddr;
    std::memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockaddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    {
        printf("connect server failed\n");
        return 1;
    }
    else
    {
        printf("connect server successfully\n");
    }

    // 3. 向服务端发送数据
    std::string data = "test";
    send(sockfd, data.c_str(), data.size(), 0);
    
    // 4. 接收服务端的数据
    char buf[1024] = {0};
    recv(sockfd, buf, sizeof(buf), 0);
    printf("recived data from server: %s\n", buf);

    // 5. 关闭 socket
    close(sockfd);
    return 0;
}