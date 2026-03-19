#include "eventloopthreadpool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, size_t numThreads)
    : baseLoop(baseLoop), numThreads(numThreads), next(0)
{
    // 预先分配线程和事件循环的空间，避免在 start() 中频繁扩容
    threads.reserve(numThreads);
    loops.reserve(numThreads);
}

void EventLoopThreadPool::start()
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        auto thread = std::make_unique<EventLoopThread>();
        EventLoop* loop = thread->startLoop();

        threads.push_back(std::move(thread));
        loops.push_back(loop);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    if (loops.empty())
    {
        return baseLoop;
    }
    EventLoop* loop = loops[next];
    next = (next + 1) % loops.size();
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if (loops.empty())
    {
        return { baseLoop };
    }
    return loops;
}