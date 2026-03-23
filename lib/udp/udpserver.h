#include "socket.h"

using namespace nl;

class UdpServer {
public:
    UdpServer() = default;
    ~UdpServer();

    UdpServer(const UdpServer&) = delete;
    UdpServer& operator=(const UdpServer&) = delete;

    void start(const std::string &ip, int port);
    void run();

private:
    Socket server;
};