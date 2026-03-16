#include "eventloopthread.h"

EventLoopThread::EventLoopThread(std::function<void(EventLoop*)> initCallback)
    : initCallback(std::move(initCallback)) {}

EventLoopThread::~EventLoopThread()
{
    if (loop)
    {
        loop->quit();
    }
    if (thread.joinable())
    {
        thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    std::unique_lock<std::mutex> lock(mutex);

    thread = std::thread([this]() {
        std::unique_ptr<EventLoop> localLoop = std::make_unique<EventLoop>();

        // 执行用户回调，进行线程特定的初始化
        if (initCallback)
        {
            initCallback(localLoop.get());
        }

        // 通知主线程：subLoop 已就绪
        {
            std::lock_guard<std::mutex> guard(mutex);
            loop = std::move(localLoop);
            cond.notify_one();
        }

        // 启动事件循环
        loop->loop();
    });

    cond.wait(lock, [this]() { return loop != nullptr; });

    return loop.get();
}
        