#include "socket.h"

using namespace nl;

class UdpClient {
public:
    UdpClient() = default;
    ~UdpClient();

    UdpClient(const UdpClient&) = delete;
    UdpClient& operator=(const UdpClient&) = delete;

    void init();
    
    ssize_t sendTo(const std::string& data, const std::string &ip, int port);
    std::string recvFrom(size_t max_len, std::string &ip, int &port);
    
private:
    Socket client;
};