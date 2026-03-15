#pragma once

#include <sys/epoll.h>
#include <vector>

class Epoll {
private:
    int epfd;
    std::vector<epoll_event> events;

public:
    Epoll(int maxEvents = 1024);
    ~Epoll();

    bool add(int fd, uint32_t event);
    bool mod(int fd, uint32_t event);
    bool del(int fd);

    int wait(int timeout = -1);

    epoll_event getEvent(int i);
};