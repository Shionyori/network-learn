#include "connection.h"

#include <cerrno>
#include <sys/epoll.h>
#include <sys/socket.h>

Connection::Connection(EventLoop* loop_, Socket&& socket_)
    : loop(loop_),
      socket(std::move(socket_)),
      channel(std::make_unique<Channel>(socket.getFd(), &socket)),
      state(State::Connecting),
      inputBuffer(),
      outputBuffer()
{
    channel->setReadCallback([this]() { handleRead(); });
    channel->setWriteCallback([this]() { handleWrite(); });
}

Connection::~Connection() = default;

void Connection::connectEstablished()
{
    setState(State::Connected);
    channel->setEvents(EPOLLIN);
    loop->addChannel(channel.get());

    if (connectionCallback)
    {
        connectionCallback(this);
    }
}

void Connection::connectDestroyed()
{
    if (state == State::Connected)
    {
        setState(State::Disconnected);
        loop->removeChannel(channel.get());
    }
}

void Connection::send(const std::string& data)
{
    if (state != State::Connected)
    {
        return;
    }

    if (loop->isInLoopThread())
    {
        sendInLoop(data.data(), data.size());
        return;
    }

    loop->runInLoop([this, data]() { sendInLoop(data.data(), data.size()); });
}

void Connection::shutdown()
{
    if (state == State::Connected)
    {
        setState(State::Disconnecting);
        if (loop->isInLoopThread())
        {
            if (outputBuffer.readableBytes() == 0)
            {
                ::shutdown(socket.getFd(), SHUT_WR);
            }
        }
        else
        {
            loop->runInLoop([this]() {
                if (outputBuffer.readableBytes() == 0)
                {
                    ::shutdown(socket.getFd(), SHUT_WR);
                }
            });
        }
    }
}

void Connection::handleRead()
{
    int savedErrno = 0;
    const ssize_t n = inputBuffer.readFd(socket.getFd(), &savedErrno);

    if (n > 0)
    {
        if (messageCallback)
        {
            messageCallback(this, &inputBuffer);
        }
        return;
    }

    if (n == 0)
    {
        handleClose();
        return;
    }

    if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK)
    {
        handleClose();
    }
}

void Connection::handleWrite()
{
    if ((channel->getEvents() & EPOLLOUT) == 0)
    {
        return;
    }

    int savedErrno = 0;
    const ssize_t n = outputBuffer.writeFd(socket.getFd(), &savedErrno);
    if (n < 0)
    {
        if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK)
        {
            handleClose();
        }
        return;
    }

    if (outputBuffer.readableBytes() == 0)
    {
        channel->setEvents(channel->getEvents() & ~EPOLLOUT);
        loop->updateChannel(channel.get());

        if (state == State::Disconnecting)
        {
            ::shutdown(socket.getFd(), SHUT_WR);
        }
    }
}

void Connection::handleClose()
{
    if (state == State::Disconnected)
    {
        return;
    }

    setState(State::Disconnected);
    loop->removeChannel(channel.get());

    if (closeCallback)
    {
        closeCallback(this);
    }
}

void Connection::sendInLoop(const char* data, size_t len)
{
    if (state == State::Disconnected)
    {
        return;
    }

    if ((channel->getEvents() & EPOLLOUT) == 0 && outputBuffer.readableBytes() == 0)
    {
        const ssize_t n = socket.send(data, len, 0);
        if (n >= 0)
        {
            const size_t sent = static_cast<size_t>(n);
            if (sent == len)
            {
                return;
            }
            outputBuffer.append(data + sent, len - sent);
        }
        else
        {
            if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
                handleClose();
                return;
            }
            outputBuffer.append(data, len);
        }
    }
    else
    {
        outputBuffer.append(data, len);
    }

    if ((channel->getEvents() & EPOLLOUT) == 0)
    {
        channel->setEvents(channel->getEvents() | EPOLLOUT);
        loop->updateChannel(channel.get());
    }
}