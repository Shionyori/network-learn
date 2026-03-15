#include "buffer.h"

#include <algorithm>
#include <cerrno>
#include <sys/uio.h>
#include <unistd.h>

Buffer::Buffer(size_t initSize) : buffer(initSize), readIndex(0), writeIndex(0) {}

Buffer::~Buffer() {}

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
    char temp_buffer[50000];
    struct iovec vec[2];
    const size_t writable = writableBytes();

    // 旧版本：&*buffer.begin() 解引用迭代器 -> char& -> &取地址 -> char*
    vec[0].iov_base = buffer.data() + writeIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = temp_buffer;
    vec[1].iov_len = sizeof(temp_buffer);

    // 读取数据（如果 buffer 空间不足则剩余数据读入 temp_buffer）
    ssize_t n = ::readv(fd, vec, 2);
    if(n < 0)
    {
        if(savedErrno) *savedErrno = errno;
        return -1;
    }

    if(static_cast<size_t>(n) <= writable)
    {
        writeIndex += n;
    }
    else
    {
        writeIndex = buffer.size();
        append(temp_buffer, static_cast<size_t>(n) - writable); // 将 temp_buffer 的数据追加到 buffer
    }
    return n;
}

ssize_t Buffer::writeFd(int fd, int* savedErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if(n < 0)
    {
        if(savedErrno) *savedErrno = errno;
        return -1;
    }
    retrieve(n);
    return n;
}

void Buffer::append(const char* data, size_t len)
{
    if(len > writableBytes())
    {
        makeSpace(len);
    }
    std::copy(data, data + len, buffer.data() + writeIndex);
    writeIndex += len;
}

void Buffer::append(const std::string& data) {
    append(data.c_str(), data.size());
}

void Buffer::append(const Buffer& data) {
    append(data.peek(), data.readableBytes());
}

void Buffer::retrieve(size_t len) 
{
    if(len >= readableBytes()) retrieveAll();
    else 
    {
        readIndex += len;
    }
}

void Buffer::retrieveAll()
{
    readIndex = 0;
    writeIndex = 0;
}

void Buffer::makeSpace(size_t len)
{
    if (writableBytes() + prependBytes() < len)
    {
        buffer.resize(writeIndex + len);
    }
    else
    {
        const size_t readable = readableBytes();
        std::copy(buffer.data() + readIndex, buffer.data() + writeIndex, buffer.data());
        readIndex = 0;
        writeIndex = readable;
    }
}