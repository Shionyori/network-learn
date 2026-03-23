#include "udpclient.h"
#include <iostream>

int main()
{
    UdpClient client;
    
    client.init();

    std::string server_ip = "127.0.0.1";
    int server_port = 8080;

    client.sendTo("Hello there!", server_ip, server_port);

    std::string server_ip_recv;
    int server_port_recv;

    std::string data = client.recvFrom(1024, server_ip_recv, server_port_recv);
    std::cout << "Received from " << server_ip_recv << ":" << server_port_recv << " - " << data << std::endl;

    return 0;
}