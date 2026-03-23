#include "socket.h"

using namespace nl;

class UdpServer {
public:
    UdpServer() = default;
    ~UdpServer();

    void start(const std::string &ip, int port);
    void run();

private:
    Socket server;
};