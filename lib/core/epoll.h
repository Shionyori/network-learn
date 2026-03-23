#pragma once

#include <sys/epoll.h>
#include <vector>
#include "channel.h"

class Epoll {
private:
    int epfd;
    std::vector<epoll_event> events;

public:
    Epoll(int maxEvents = 1024);
    ~Epoll();

    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;

    bool add(Channel* channel);
    bool mod(Channel* channel);
    bool del(Channel* channel);

    int wait(int timeout = -1);

    epoll_event getEvent(int i) const;
    Channel* getChannel(int i) const;
};