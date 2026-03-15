#pragma once

#include <functional>
#include <sys/epoll.h>
#include "socket.h"

using namespace nl;

class Channel {
private:
    int fd;

    uint32_t events;
    uint32_t revents;

    Socket* socket;

    std::function<void()> readCallback;
    std::function<void()> writeCallback;

public:
    Channel(int fd, Socket* sock = nullptr) : fd(fd), socket(sock), events(0), revents(0) {}

    // 禁止拷贝，允许移动
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    Channel(Channel&&) = default;
    Channel& operator=(Channel&&) = default;

    int getFd() const { return fd; }
    Socket* getSocket() const { return socket; }

    uint32_t getEvents() const { return events; }
    uint32_t getRevents() const { return revents; }
    
    void setEvents(uint32_t ev) { events = ev; }
    void setRevents(uint32_t rev) { revents = rev; }

    void setReadCallback(const std::function<void()>& cb) { readCallback = cb; }
    void setWriteCallback(const std::function<void()>& cb) { writeCallback = cb; }
    

    void handleEvent() {
        if ((revents & EPOLLIN) && readCallback) {
            readCallback();
        }
        if ((revents & EPOLLOUT) && writeCallback) {
            writeCallback();
        }
    }
};