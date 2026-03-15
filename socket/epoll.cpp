#include "epoll.h"
#include <unistd.h>

Epoll::Epoll(int maxEvents)
{
    epfd = epoll_create1(0);
    events.resize(maxEvents);
}

Epoll::~Epoll()
{
    close(epfd);
}

bool Epoll::add(int fd, uint32_t event)
{
    epoll_event ev{};
    ev.events = event;
    ev.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoll::mod(int fd, uint32_t event)
{
    epoll_event ev{};
    ev.events = event;
    ev.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoll::del(int fd)
{
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr) == 0;
}

int Epoll::wait(int timeout)
{
    return epoll_wait(epfd, events.data(), events.size(), timeout);
}

epoll_event Epoll::getEvent(int i)
{
    return events[i];
}