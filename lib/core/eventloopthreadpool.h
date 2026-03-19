#pragma once

#include "eventloop.h"
#include "eventloopthread.h"
#include <vector>
#include <memory>

class EventLoopThreadPool {
private:
    EventLoop* baseLoop;
    ssize_t numThreads;
    std::vector<std::unique_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops;
    size_t next; // 轮询索引

public:
    EventLoopThreadPool(EventLoop* baseLoop, size_t numThreads);
    ~EventLoopThreadPool() = default;

    void start();
    
    EventLoop* getNextLoop();
    std::vector<EventLoop*> getAllLoops();
};