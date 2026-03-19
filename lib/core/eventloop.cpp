#include "eventloop.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <cerrno>
#include <cstdint>

EventLoop::EventLoop(int maxEvents)
    : epoll(maxEvents),
      looping(false),
      isQuit(false),
      callingPendingFunctors(false),
      threadId(),
      wakeupFd(-1)
{
    wakeupFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeupFd >= 0)
    {
        wakeupChannel = std::make_unique<Channel>(wakeupFd);
        wakeupChannel->setEvents(EPOLLIN);
        wakeupChannel->setReadCallback([this]()
                                       { handleWakeupRead(); });
        epoll.add(wakeupChannel.get());
    }
}

EventLoop::~EventLoop()
{
    if (wakeupChannel)
    {
        epoll.del(wakeupChannel.get());
    }

    if (wakeupFd >= 0)
    {
        close(wakeupFd);
    }
}

void EventLoop::loop()
{
    threadId = std::this_thread::get_id();
    looping = true;
    isQuit = false;

    while (!isQuit)
    {
        int n = epoll.wait(-1); // 阻塞等待事件发生
        activeChannels.clear();

        // 将就绪事件对应的 Channel 添加到 activeChannels 中
        for (int i = 0; i < n; i++)
        {
            Channel *channel = epoll.getChannel(i);
            if (channel)
            {
                channel->setRevents(epoll.getEvent(i).events);
                activeChannels.push_back(channel);
            }
        }
        // 处理所有就绪事件
        for (Channel *channel : activeChannels)
        {
            channel->handleEvent();
        }
        // 处理 pendingFunctors 中的任务
        doPendingFunctors();
    }
    looping = false;
}

void EventLoop::addChannel(Channel *channel)
{
    if (!channel)
    {
        return;
    }
    epoll.add(channel);
}

void EventLoop::updateChannel(Channel *channel)
{
    if (!channel)
    {
        return;
    }
    epoll.mod(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    if (!channel)
    {
        return;
    }
    epoll.del(channel);
}

void EventLoop::quit()
{
    isQuit = true;
    wakeup();
}

void EventLoop::runInLoop(const std::function<void()> &cb)
{
    if (!cb)
    {
        return;
    }
    if (isInLoopThread())
    {
        cb();
        return;
    }

    queueInLoop(cb);
}

void EventLoop::queueInLoop(const std::function<void()> &cb)
{
    if (!cb)
    {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(pendingMutex);
        pendingFunctors.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors.load())
    {
        wakeup();
    }
}

bool EventLoop::isInLoopThread() const
{
    return threadId == std::this_thread::get_id();
}

void EventLoop::wakeup()
{
    if (wakeupFd < 0)
    {
        return;
    }

    uint64_t one = 1;
    ssize_t n = write(wakeupFd, &one, sizeof(one));
    (void)n;
}

void EventLoop::handleWakeupRead()
{
    if (wakeupFd < 0)
    {
        return;
    }

    uint64_t value;
    while (true)
    {
        ssize_t n = read(wakeupFd, &value, sizeof(value));
        if (n == sizeof(value))
        {
            continue;
        }
        if (n < 0 && errno == EAGAIN)
        {
            break;
        }
        break;
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<std::function<void()>> functors;
    callingPendingFunctors = true;

    {
        std::lock_guard<std::mutex> lock(pendingMutex);
        functors.swap(pendingFunctors);
    }

    for (const auto &fn : functors)
    {
        fn();
    }

    callingPendingFunctors = false;
}