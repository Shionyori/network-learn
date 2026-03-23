#pragma once

#include <cstddef>
#include <sys/types.h>
#include <vector>
#include <string>

class Buffer {
private:
    std::vector<char> buffer;
    size_t readIndex;
    size_t writeIndex;

public:
    Buffer(size_t initSize = 1024);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);

    ssize_t readFd(int fd, int* savedErrno);
    ssize_t writeFd(int fd, int* savedErrno);

    void append(const char* data, size_t len);
    void append(const std::string& data);
    void append(const Buffer& data);

    const char* peek() const { return buffer.data() + readIndex; }
    
    void retrieve(size_t len);
    void retrieveAll();

    size_t readableBytes() const { return writeIndex - readIndex; }
    size_t writableBytes() const { return buffer.size() - writeIndex; }
    size_t prependBytes() const { return readIndex; }

private:
    void makeSpace(size_t len);
};