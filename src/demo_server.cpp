#include "tcpserver.h"
#include <iostream>

int main()
{
    TcpServer server;

    server.start("127.0.0.1", 8080);
    server.run();

    return 0;
}