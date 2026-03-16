#pragma once

#include "eventloop.h"
#include <thread>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

class EventLoopThread {
private:
    std::unique_ptr<EventLoop> loop;
    std::thread thread;
    std::mutex mutex;
    std::condition_variable cond;
    std::function<void(EventLoop*)> initCallback;

public:
    EventLoopThread(std::function<void(EventLoop*)> initCallback = nullptr);
    ~EventLoopThread();

    EventLoopThread(const EventLoopThread&) = delete;
    EventLoopThread& operator=(const EventLoopThread&) = delete;

    EventLoop* startLoop(); // 启动事件循环线程
};