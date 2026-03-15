#include "tcpclient.h"

#include <iostream>

int main()
{
    TcpClient client;
    client.connect("127.0.0.1", 8080);
    client.send("Hello there!");
    std::string data = client.recv();
    std::cout << "Data received from server: " << data << std::endl;

    return 0;
}