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

bool Epoll::add(Channel* channel)
{
    epoll_event ev{};
    ev.events = channel->getEvents();
    ev.data.ptr = channel;

    return epoll_ctl(epfd, EPOLL_CTL_ADD, channel->getFd(), &ev) == 0;
}

bool Epoll::mod(Channel* channel)
{
    epoll_event ev{};
    ev.events = channel->getEvents();
    ev.data.ptr = channel;

    return epoll_ctl(epfd, EPOLL_CTL_MOD, channel->getFd(), &ev) == 0;
}

bool Epoll::del(Channel* channel)
{
    return epoll_ctl(epfd, EPOLL_CTL_DEL, channel->getFd(), nullptr) == 0;
}

int Epoll::wait(int timeout)
{
    return epoll_wait(epfd, events.data(), events.size(), timeout);
}

epoll_event Epoll::getEvent(int i) const
{
    return events[i];
}

Channel* Epoll::getChannel(int i) const
{
    return static_cast<Channel*>(events[i].data.ptr);
}
