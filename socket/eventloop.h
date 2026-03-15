#pragma once

#include "epoll.h"
#include <vector>
#include "channel.h"
#include <atomic>
#include <thread>
#include <functional>
#include <memory>
#include <mutex>

class EventLoop {
private:
    Epoll epoll;

    std::vector<Channel*> activeChannels;

    std::atomic<bool> looping;
    std::atomic<bool> isQuit;
    std::atomic<bool> callingPendingFunctors;

    std::thread::id threadId;

    int wakeupFd;
    std::unique_ptr<Channel> wakeupChannel;

    std::mutex pendingMutex;
    std::vector<std::function<void()>> pendingFunctors;

public:
    EventLoop(int maxEvents = 1024);
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    void loop();
    void quit();

    void addChannel(Channel* channel);
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void runInLoop(const std::function<void()>& cb);
    void queueInLoop(const std::function<void()>& cb);

    bool isInLoopThread() const;

private:
    void wakeup();
    void handleWakeupRead();
    void doPendingFunctors();
};