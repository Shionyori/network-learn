#include "tcpserver.h"
#include <iostream>

int main()
{
    TcpServer server;

    server.start("127.0.0.1", 8080);

    server.run([](Socket client) {
        std::cout << "Client connected: fd=" << client.getFd() << std::endl;
        std::string data = client.recv();
        std::cout << "Received from client: " << data << std::endl;
        client.send("Echo: " + data);
    });

    return 0;
}